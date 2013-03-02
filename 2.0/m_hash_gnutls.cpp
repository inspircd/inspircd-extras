/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2012 Attila Molnar <attilamolnar@hush.com>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $ModAuthor: Attila Molnar */
/* $ModAuthorMail: attilamolnar@hush.com */
/* $ModDesc: Implements hash functions using GnuTLS API */
/* $ModDepends: core 2.0 */
/* $ModConflicts: m_sha256.so */
/* $ModConflicts: m_ripemd160.so */

#include "inspircd.h"
#include "hash.h"
#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>

/* $CompileFlags: pkgconfincludes("gnutls","/gnutls/gnutls.h","") */
/* $LinkerFlags: rpath("pkg-config --libs gnutls") pkgconflibs("gnutls","/libgnutls.so","-lgnutls") */
/* $NoPedantic */

class GnuTLSHash : public HashProvider
{
	const gnutls_digest_algorithm_t algo;
 public:
	GnuTLSHash(Module* parent, const std::string& Name, const size_t outputsize, const size_t blocksize, gnutls_digest_algorithm_t digestalgo)
		: HashProvider(parent, Name, outputsize, blocksize), algo(digestalgo)
	{
	}

	std::string sum(const std::string& data)
	{
		char digest[this->out_size];
		gnutls_hash_fast(algo, data.data(), data.length(), (unsigned char*)digest);
		return std::string(digest, this->out_size);
	}

	std::string sumIV(unsigned int* IV, const char* HexMap, const std::string &sdata)
	{
		return "";
	}
};

class ModuleHashGnuTLS : public Module
{
	GnuTLSHash sha1;
	GnuTLSHash sha256;
	GnuTLSHash sha512;
	GnuTLSHash ripemd160;

 public:
	ModuleHashGnuTLS()
		: sha1(this, "hash/sha1", 20, 64, GNUTLS_DIG_SHA1)
		, sha256(this, "hash/sha256", 32, 64, GNUTLS_DIG_SHA256)
		, sha512(this, "hash/sha512", 64, 128, GNUTLS_DIG_SHA512)
		, ripemd160(this, "hash/ripemd160", 20, 64, GNUTLS_DIG_RMD160)
	{
		gnutls_global_init();
	}

	~ModuleHashGnuTLS()
	{
		gnutls_global_deinit();
	}

	void init()
	{
		ServiceProvider* services[] = { &sha1, &sha256, &sha512, &ripemd160 };
		ServerInstance->Modules->AddServices(services, sizeof(services)/sizeof(ServiceProvider*));
	}

	Version GetVersion()
	{
		return Version("Implements hash functions using GnuTLS API", VF_VENDOR);
	}
};

MODULE_INIT(ModuleHashGnuTLS)
