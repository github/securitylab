import cpp

/** 
 * Common utilities for OpenSSL certificate verification.
 */

class SslSetVerifyFunctionCall extends FunctionCall {
  SslSetVerifyFunctionCall() { this.getTarget().hasName("SSL_set_verify") }
}

class SslCtxSetVerifyFunctionCall extends FunctionCall {
  SslCtxSetVerifyFunctionCall() { this.getTarget().hasName("SSL_CTX_set_verify") }
}

class SslLikeSetVerifyFunctionCall extends FunctionCall {
  SslLikeSetVerifyFunctionCall() {
    this instanceof SslCtxSetVerifyFunctionCall or
    this instanceof SslSetVerifyFunctionCall
  }
}

class SslCtxSetCertVerifyCallbackFunctionCall extends FunctionCall {
  SslCtxSetCertVerifyCallbackFunctionCall() {
    this.getTarget().hasName("SSL_CTX_set_cert_verify_callback")
  }
}
