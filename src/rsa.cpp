// The MIT License (MIT)
//
// Copyright (c) 2016 Artur Troian
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>

#include <josepp/crypto.hpp>
#include <josepp/b64.hpp>
#include <josepp/statics.hh>

namespace jose {

rsa::rsa(jose::alg alg, sp_rsa_key key)
	: crypto(alg)
	, _r(key)
{
	if (alg != jose::alg::RS256 && alg != jose::alg::RS384 && alg != jose::alg::RS512) {
		throw std::invalid_argument("Invalid algorithm");
	}

	_key_size = static_cast<unsigned int>(RSA_size(_r.get()));
}

rsa::~rsa() {
	static_instance();
}

std::string rsa::sign(const std::string &data) {
	std::shared_ptr<uint8_t> sig = std::shared_ptr<uint8_t>(new uint8_t[_key_size], std::default_delete<uint8_t[]>());

	digest d(_hash_type, reinterpret_cast<const uint8_t *>(data.data()), data.length());

	if (RSA_sign(hash2nid(_hash_type), d.data(), static_cast<int>(d.size()), sig.get(), &_key_size, _r.get()) != 1) {
		throw std::runtime_error("Couldn't sign RSA");
	}

	return b64::encode_uri(sig.get(), _key_size);
}

bool rsa::verify(const std::string &data, const std::string &sig) {
	digest d(_hash_type, reinterpret_cast<const uint8_t *>(data.data()), data.length());

	std::vector<uint8_t> s = b64::decode_uri(sig.data(), sig.length());

	return RSA_verify(
		hash2nid(_hash_type)
		, d.data()
		, static_cast<int>(d.size())
		, reinterpret_cast<const uint8_t *>(s.data())
		, static_cast<int>(s.size())
		, _r.get()) == 1;
}

sp_rsa_key rsa::gen(int size) {
	// keys less than 1024 bits are insecure
	if ((size % 1024) != 0) {
		throw std::invalid_argument("Invalid keys size");
	}

	sp_rsa_key key = std::shared_ptr<RSA>(RSA_new(), ::RSA_free);
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);
	RSA_generate_key_ex(key.get(), size, bn, nullptr);

	return key;
}

sp_rsa_key rsa::load_from_file(const std::string &path, password_cb on_password) {
	RSA *r;

	auto f = std::fopen(path.c_str(), "r");
	if (!f) {
		throw std::runtime_error("cannot open file");
	}

	on_password_wrap wrap(on_password);

	r = PEM_read_RSAPrivateKey(f, nullptr, password_loader, &wrap);
	if (wrap.required) {
		throw std::runtime_error("password required");
	} else if (r == nullptr) {
		throw std::runtime_error("read rsa key");
	}

	return std::shared_ptr<RSA>(r, ::RSA_free);
}

int rsa::password_loader(char *buf, int size, int rwflag, void *u) {
	auto wrap = reinterpret_cast<on_password_wrap *>(u);

	if (wrap->cb == nullptr) {
		wrap->required = true;
		return 0;
	}

	secure_string pass;
	int pass_size = 0;

	try {
		wrap->cb(pass, rwflag);
		pass_size = pass.copy(buf, secure_string::size_type(size), 0);
	} catch (...) {
		pass_size = 0;
	}

	return pass_size;
}

} // namespace jose
