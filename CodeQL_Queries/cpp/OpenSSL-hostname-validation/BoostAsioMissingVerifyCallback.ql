/**
 * @name BoostAsioMissingVerifyCallback
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/boost-asio-missing-boost-verify-callback
 * @tags security
 *       external/cwe/cwe-273
 */

/*
 * The default `verify_callback` https://www.openssl.org/docs/man1.1.1/man3/SSL_CTX_set_verify.html
 * does not compare CN/hostnames so a TLS client can be MITMed.
 *
 * The boost ASIO/SSL library also does not install a hostname verifier callback.
 *
 * False positives include intentionally skipping checking (for example as the result of
 * configuration), using a server context, or using an uncommon strategy for inspecting certs.
 */

import cpp

predicate sslOrDetailNamespace(Namespace n) {
  n.getQualifiedName() = "boost::asio::ssl" or
  n.getQualifiedName() = "boost::asio::ssl::detail"
}

predicate sslStreamOrEngineType(UserType t) {
  t.getSimpleName() = "stream" or
  t.getSimpleName() = "engine" or
  t.getSimpleName() = "context"
}

class AsioStreamType extends Type {
  Type baseType;

  AsioStreamType() {
    (
      // Consider shared_ptr, raw pointers, or base types.
      baseType = this.(DerivedType).getBaseType() or
      baseType = this.(UserType).getATemplateArgument+() or
      baseType = this
    ) and
    sslOrDetailNamespace(baseType.(UserType).getNamespace()) and
    sslStreamOrEngineType(baseType)
  }
}

class StreamSocketVariable extends Variable {
  AsioStreamType type;

  StreamSocketVariable() { this.getType() = type }
}

class SetVerifyModeFunctionCall extends FunctionCall {
  SetVerifyModeFunctionCall() {
    // This can be improved, false positives can be reduced by also checking namespace.
    this.getTarget().hasName("set_verify_mode")
  }
}

class SetVerifyCallbackFunctionCall extends FunctionCall {
  SetVerifyCallbackFunctionCall() {
    // This can be improved, false positives can be reduced by also checking namespace.
    this.getTarget().hasName("set_verify_callback")
  }
}

predicate callsSetVerifyCallback(StreamSocketVariable v) {
  exists(SetVerifyCallbackFunctionCall fc, Expr e |
    e = fc.getQualifier() and
    v = e.(VariableAccess).getTarget()
  )
}

from SetVerifyModeFunctionCall fc, StreamSocketVariable v
where
  // There is most likely a better method than assuming the only namespace/simple-name
  // child access is the qualifier.
  v = fc.getQualifier().(VariableAccess).getTarget() and
  not callsSetVerifyCallback(v)
select fc,
  "boost::asio::ssl context calls set_verify_mode without a callback $@, if this is a client context then hostname validation may be missing",
  v, "on the socket (" + v.getName() + ")"
