//
// Created by Artur Troian on 1/20/17.
//
#pragma once

#include <memory>

#include <json/json.h>

namespace jwt {

using sp_claims = std::shared_ptr<class claims>;

/**
 * \brief
 */
class claims final {
private:
	class has {
	public:
		explicit has(Json::Value *c) : claims_(c) {}
	public:
		bool any(const std::string &key) { return claims_->isMember(key); }
		bool iss() { return any("iss"); }
		bool sub() { return any("sub"); }
		bool aud() { return any("aud"); }
		bool exp() { return any("exp"); }
		bool nbf() { return any("nbf"); }
		bool iat() { return any("nbf"); }
		bool jti() { return any("jti"); }
	private:
		Json::Value *claims_;
	};

	class check {
	public:
		explicit check(Json::Value *c) : claims_(c) {}
	public:
		bool any(const std::string &key, const std::string &value) {
			std::string s = claims_->operator[](key).asString();
			return s == value;
		}
		bool iss(const std::string &value) { return any("iss", value); }
		bool sub(const std::string &value) { return any("sub", value); }
		bool aud(const std::string &value) { return any("aud", value); }
		bool exp(const std::string &value) { return any("exp", value); }
		bool nbf(const std::string &value) { return any("nbf", value); }
		bool iat(const std::string &value) { return any("iat", value); }
		bool jti(const std::string &value) { return any("iat", value); }
	private:
		Json::Value *claims_;
	};

	class del {
	public:
		explicit del(Json::Value *c) : claims_(c) {}
	public:
		void any(const std::string &key) { claims_->removeMember(key); }
		void iss() { any("iss"); }
		void sub() { any("sub"); }
		void aud() { any("aud"); }
		void exp() { any("exp"); }
		void nbf() { any("nbf"); }
		void iat() { any("nbf"); }
		void jti() { any("jti"); }
	private:
		Json::Value *claims_;
	};


	class get {
	public:
		explicit get(Json::Value *c) : claims_(c) {}
	public:
		std::string any(const std::string &key) {
			std::string s = claims_->operator[](key).asString();
			return std::move(s);
		}
		std::string iss() { return std::move(any("iss")); }
		std::string sub() { return std::move(any("sub")); }
		std::string aud() { return std::move(any("aud")); }
		std::string exp() { return std::move(any("exp")); }
		std::string nbf() { return std::move(any("nbf")); }
		std::string iat() { return std::move(any("iat")); }
		std::string jti() { return std::move(any("iat")); }
	private:
		Json::Value *claims_;
	};

	class set {
	public:
		explicit set(Json::Value *c) : claims_(c) {}
	public:
		void any(const std::string &key, const std::string &value);
		void iss(const std::string &value) { any("iss", value); }
		void sub(const std::string &value) { any("sub", value); }
		void aud(const std::string &value) { any("aud", value); }
		void exp(const std::string &value) { any("exp", value); }
		void nbf(const std::string &value) { any("nbf", value); }
		void iat(const std::string &value) { any("iat", value); }
		void jti(const std::string &value) { any("jti", value); }

	private:
		Json::Value *claims_;
	};
public:
	/**
	 * \brief
	 */
	claims();

	/**
	 * \brief
	 *
	 * \param d
	 */
	explicit claims(const std::string &d, bool b64 = false);

	/**
	 * \brief
	 *
	 * \param key
	 * \param value
	 *
	 * \return
	 */
	class claims::set &set() { return set_; }

	/**
	 * \brief
	 *
	 * \param key
	 *
	 * \return
	 */
	class claims::has &has() { return has_; }

	/**
	 * \brief
	 *
	 * \param key
	 *
	 * \return
	 */
	class claims::del &del() { return del_; }

	/**
	 * \brief
	 *
	 * \param key
	 *
	 * \return
	 */
	class claims::get &get() { return get_; }

	class claims::check &check() { return check_; }

	std::string b64();

private:
	Json::Value claims_;

	class set   set_;
	class get   get_;
	class has   has_;
	class del   del_;
	class check check_;
};

} // namespace jwt
