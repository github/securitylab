/**
 * @name OpenSSLMissingCNCheck
 * @kind path-problem
 * @problem.severity recommendation
 * @id cpp/openssl-missing-verify-callback
 * @tags security
 *       external/cwe/cwe-273
 */

/*
 * The default `verify_callback` https://www.openssl.org/docs/man1.1.1/man3/SSL_CTX_set_verify.html
 * does not compare CN/hostnames so a TLS client can be MITMed.
 *
 * There are multiple stategies for verifying server certificates so this query assists with code
 * review but does not accurately find vulnerabilities, false positives are likely.
 *
 * False negatives include using verify_callback to ignore verification, such as always returning
 * a 1 without checking `preverify_ok`.
 */

import cpp
import semmle.code.cpp.dataflow.TaintTracking
import OpenSSLVerify

class SslSet1HostFunctionCall extends FunctionCall {
  SslSet1HostFunctionCall() { this.getTarget().hasName("SSL_set1_host") }
}

class SslLikeCheckHostnameFunctionCall extends FunctionCall {
  SslLikeCheckHostnameFunctionCall() {
    this instanceof SslSet1HostFunctionCall or
    this instanceof SslCtxSetCertVerifyCallbackFunctionCall
  }
}

class SslPointerVariable extends Variable {
  SslPointerVariable() {
    this.hasDefinition() and this.getUnderlyingType().(PointerType).getBaseType().hasName("SSL")
  }
}

class SslCtxPointerVariable extends Variable {
  SslCtxPointerVariable() {
    this.hasDefinition() and this.getUnderlyingType().(PointerType).getBaseType().hasName("SSL_CTX")
  }
}

class SslLikePointerVariable extends Variable {
  SslLikePointerVariable() {
    this instanceof SslPointerVariable or
    this instanceof SslCtxPointerVariable
  }
}

class SslCtxNewClientFunctionCall extends FunctionCall {
  SslCtxNewClientFunctionCall() {
    this.getTarget().hasName("SSL_CTX_new") and
    exists(FunctionCall fc |
      fc.getTarget().getQualifiedName().matches("%_client_method") and
      TaintTracking::localTaint(DataFlow::exprNode(fc), DataFlow::exprNode(this.getArgument(0)))
    )
  }
}

predicate sslPointerTaintStep(DataFlow::Node pred, DataFlow::Node succ) {
  // Propagate the taint from SSL <-> SSL_CTX.
  exists(SslLikePointerVariable ctx, FunctionCall fc |
    pred.asExpr() = ctx.getAnAccess() and
    pred.asExpr() = fc.getArgument(0) and
    succ.asExpr() = fc and
    // Simple to go back and forth.
    (fc.getTarget().hasName("SSL_new") or fc.getTarget().hasName("SSL_get_SSL_CTX"))
  )
}

class SslCtxSetVerifyConfiguration extends TaintTracking::Configuration {
  SslCtxSetVerifyConfiguration() { this = "SslCtxSetVerifyConfiguration" }

  override predicate isSource(DataFlow::Node node) {
    node.asExpr() instanceof SslCtxNewClientFunctionCall
  }

  override predicate isSink(DataFlow::Node node) {
    // False positives can be reduced by checking that argument(1) is not VERIFY_NONE.
    // This may lead to broader false negatives where code mistakenly sets VERIFY_NONE.
    exists(SslLikeSetVerifyFunctionCall fc | node.asExpr() = fc.getArgument(0))
  }

  override predicate isAdditionalTaintStep(DataFlow::Node pred, DataFlow::Node succ) {
    sslPointerTaintStep(pred, succ)
  }
}

class SslCtxCheckHostnameConfiguration extends TaintTracking::Configuration {
  SslCtxCheckHostnameConfiguration() { this = "SslCtxCheckHostnameConfiguration" }

  override predicate isSource(DataFlow::Node node) {
    node.asExpr() instanceof SslCtxNewClientFunctionCall
  }

  override predicate isSink(DataFlow::Node node) {
    exists(SslLikeCheckHostnameFunctionCall fc | node.asExpr() = fc.getArgument(0))
  }

  override predicate isAdditionalTaintStep(DataFlow::Node pred, DataFlow::Node succ) {
    sslPointerTaintStep(pred, succ)
  }
}

predicate flowsToSetVerify(FunctionCall ctx, FunctionCall fc) {
  exists(SslCtxSetVerifyConfiguration conf |
    conf.hasFlow(DataFlow::exprNode(ctx), DataFlow::exprNode(fc.getArgument(0)))
  )
}

predicate flowsToCheckHostname(FunctionCall ctx) {
  exists(SslCtxCheckHostnameConfiguration conf, FunctionCall fc |
    conf.hasFlow(DataFlow::exprNode(ctx), DataFlow::exprNode(fc.getArgument(0)))
  )
}

from FunctionCall ctx, FunctionCall fc
where
  flowsToSetVerify(ctx, fc) and
  not flowsToCheckHostname(ctx) and
  // Empty verify callback (this will not check CN).
  // False positives include intentional skipped verification and any
  // additional custom checking via accessing the peer certificate post-handshake.
  fc.getArgument(2).getValue() = "0"
select ctx, ctx, fc,
  "may be a client context without hostname verification because SSL_set1_host " +
    "and other indicators of leaf cert hostname checking are missing"
