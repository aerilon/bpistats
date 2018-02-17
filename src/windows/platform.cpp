#include <platform.hpp>
#include <wincrypt.h>

namespace bpi::platform
{

namespace ssl
{

// XXX - from https://stackoverflow.com/questions/39772878/reliable-way-to-get-root-ca-certificates-on-windows
boost::asio::ssl::context
get_default_context()
{
	boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);

	ctx.set_options(boost::asio::ssl::context::default_workarounds);

	HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
	if (hStore == nullptr) {
		return ctx;
	}

	auto store = X509_STORE_new();

	PCCERT_CONTEXT pContext = nullptr;

	while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != nullptr) {
		X509 *x509 = d2i_X509(NULL,
			const_cast<const BYTE **>(&pContext->pbCertEncoded),
			pContext->cbCertEncoded);
		if(x509 != nullptr) {
			X509_STORE_add_cert(store, x509);
			X509_free(x509);
		}
	}

	CertFreeCertificateContext(pContext);

	CertCloseStore(hStore, 0);

	SSL_CTX_set_cert_store(ctx.native_handle(), store);

	return ctx;
}

}

}

