/**
 * @name OpenSSLPreverifyIgnored
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/openssl-preverify-ignored
 * @tags security
 *       external/cwe/cwe-273
 *
 * The `preverified` variable must be checked in verify_callbacks.
 *
 * False positives include callbacks used when certificate checking is intentionally
 * bypassed or when the checking is performed on client certificates.
 */

import cpp
import OpenSSLVerify

class SetVerifyCallbackFunctionCall extends FunctionCall {
  SetVerifyCallbackFunctionCall() { this.getTarget().hasName("set_verify_callback") }
}

class CallbackArg extends Expr {
  CallbackArg() {
    exists(SetVerifyCallbackFunctionCall fc | this = fc.getArgument(0)) or
    exists(SslCtxSetCertVerifyCallbackFunctionCall fc | this = fc.getArgument(1)) or
    exists(SslCtxSetVerifyFunctionCall fc | this = fc.getArgument(2)) or
    exists(SslSetVerifyFunctionCall fc | this = fc.getArgument(2))
  }
}

class CallbackFunc extends Function {
  CallbackFunc() {
    exists(CallbackArg arg |
      // False negatives include lambdas and any allocation expression.
      this = arg.getAChild().(FunctionAccess).getTarget() or
      this = arg.(FunctionAccess).getTarget()
    )
  }
}

predicate noPreverifyAccess(Function f) {
  // False negatives include the variable being accessed but not influencing the return.
  not exists(VariableAccess va | va = f.getParameter(0).getAnAccess())
}

from CallbackFunc f
where noPreverifyAccess(f)
select f, "is used as a certificate verify callback and does not use the preverified variable"
