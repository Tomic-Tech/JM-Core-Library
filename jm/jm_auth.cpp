#include "jm_auth.hpp"

#include <iostream>
#include <fstream>
#include <crypt/rsa.h>
#include <crypt/osrng.h>
#include <crypt/base64.h>
#include <crypt/files.h>

#ifdef JM_OS_WIN
#include <jm/win32/jm_disk_info.hpp>
#endif

namespace jm {

using namespace std;
using namespace CryptoPP;
void auth::test() {
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	const Integer& n = params.GetModulus();
	const Integer& p = params.GetPrime1();
	const Integer& q = params.GetPrime2();
	const Integer& d = params.GetPrivateExponent();
	const Integer& e = params.GetPublicExponent();
	const Integer& dp = params.GetModPrime1PrivateExponent();
	const Integer& dq = params.GetModPrime2PrivateExponent();
	const Integer& u = params.GetMultiplicativeInverseOfPrime2ModPrime1();

	cout << "RSA Parameters:" << endl;
	cout << " n: " << n << endl;
	cout << " p: " << p << endl;
	cout << " q: " << q << endl;
	cout << " d: " << d << endl;
	cout << " e: " << e << endl;
	cout << " dp: " << dp << endl;
	cout << " dq: " << dq << endl;
	cout << " u: " << u << endl;
	cout << endl;

	RSA::PrivateKey privateKey(params);
	RSA::PublicKey publicKey(params);

	string plain = "RSA Encryption", cipher, recovered;
	cout << plain << endl;

	RSAES_OAEP_SHA_Encryptor e_(publicKey);

	StringSource(plain, true, new PK_EncryptorFilter(rng, e_,
		new StringSink(cipher)));

	cout << cipher << endl;

	RSAES_OAEP_SHA_Decryptor d_(privateKey);

	StringSource(cipher, true, new PK_DecryptorFilter(rng, d_,
		new StringSink(recovered)));
	StringSource(cipher, true, new Base64Encoder(new FileSink("E:/base64-encoded.dat")));

	string cipher2;
	FileSource("E:/base64-encoded.dat", true, new Base64Decoder(new StringSink(cipher2)));

	cout << cipher2 << endl;

	string recovered2;
	StringSource(cipher2, true, new PK_DecryptorFilter(rng, d_,
		new StringSink(recovered2)));
	

	cout << recovered << endl;

	cout << recovered2 << endl;
	cout << "Recovered plain text" << endl;

	fstream ss("E:/reg.dat", fstream::in | fstream::out);
	ss << cipher;
	ss.close();

	DiskInfo& di = DiskInfo::GetDiskInfo();
	long count = di.LoadDiskInfo();

	for (long i = 0; i < count; i++) {
		cout << di.SerialNumber(i) << endl;
		cout << di.DriveType(i) << endl;
		cout << di.ModelNumber(i) << endl;
		cout << di.RevisionNumber(i) << endl;
		cout << di.DriveSize(i) << endl;
		cout << di.BufferSize(i) << endl;
	}
    return;   // Program successfully completed.
}
}