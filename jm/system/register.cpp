#include "register.hpp"

#ifdef BOOST_WINDOWS
#include <jm/system/detail/windiskinfo.hpp>
#else
#include <jm/system/detail/unixdiskinfo.hpp>
#endif

#include <cstring>
#include <cstdlib>
#include <openssl/rsa.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include "cyo/CyoEncode.h"
#include "cyo/CyoDecode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>
#include <boost/smart_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>


namespace JM
{
namespace System
{
typedef JM::System::Detail::DiskInfo DiskInfo;
// N: 24865838231897529903043050252701387253817865876364150273686376242271804951752976717955506029226974144225261499246411474709884093909946483251447136883715118388437747029322866615464548666109496487856142037527178830201732314056967877269711283129448727523577745411248291349123769974972589413244225934373110145113902208372629815715926842645686544094355924878749994919806498882055230678098313003210581654472941328838351200093577363117474782945722225781874995522624003007178253827302337718798151562458306447138685650273197566918451172386803112726493002505376037971786101479879677584282206073380351557773490273894583472093793
// E: 5847329
// D: 6448696694119933705849322878430049428162598649916424661179499756954949001203128988651625277966242292705871766230910763248477291923402101172647802448688707098477970173413277265880117578030883723496748593410062128193305996166421385846151303789123845461879146968199498254096981072200201729743225830601373146764382109349999326892669327504005336124069199697096691045082172931807888469249032048586819506559915113609021112542258736369563409822625288504429743321702453794757127566924125280878483836287188789107351426428852357183920162464424826164069390372427948972123798569313462566926706945429860433093789010319911003283729
// the encryption 64base text is:
// 1. ID code generate with Hardware SN \n
// 2. Commbox Customer ID CAEN/CACN/XXXXXXXXXXX \n
// 3. First Regisiter Time \n
// 4. Expired Update Time \n
// 5. The database password \n
// 6. The language \n
// Only the ID Code get from client computer;

static const char *_theN = "2440816252823285233810869578522396920937064836015308255025770116368471245238891377886655827863376547145602207235628667347169214925277619850649995016745222927366751874925959500368072599272967947348164345202557296918419996274561401818437741790994828240029939900490934660468635215915404774173966828233781157191084398787403676734374012691352723866261601858436147554685626667100495459169468371845966144026053177582073177384843908210611713622391143035267966171887771226193701917268336152595404283772272552530587972724856401614254788279018324395192839766490947448947327518591481635234241242856913646397133141203134853111839980524200985357427602090821751721953922361894523664850608867594845069649355658932045897783784197939744921498830761459381818729095253263804667889049688321534000932261304558719655241477629934113342581863585365419210706069231547272336731161679447444718222924352762023292058718215877940949451532527692847010638070973127382245736872772360423353378741885799080125412556722465786330063424777133583648558655107227830199572536967941889445212117622378063890739151029762267439730070328530352367261063374927218708461130115467697807995697573588041228824260461037735388504375811525072787730489406257830809245538033407225099235719637";
static const char *_theE = "520867428764322287956";
static const char *_theD = "6631469748128439641093466942634298934310183617519785922832753364779504545442819243929372396326252892806279538949438336090451052942419560485725808425139665452314595491508746653641979734940110006508423323894853626068431652144372809462467897929315717758496041997753944167900641739951095600981663228301603973102328728356949181328932687752511026655924715687291073657115904361406992272742712103290190582340730909335899869474441882750868966612226430720681984316990446720160233780254640558658477370027999245397971141960223503631489607372083144501541140137376902158628244247383822407112867446141748459602582988244740708147243041490038704478674187516912473640086150475883413416419043559218279857692277263225321885288510353194417297415550578038891059137840689894680913692729327023002460593466290787647478860545291703776439619530111067066172349111222858813180998943099392501345560985497776043124669696919430128651859331884305473802348916202196552891884368907863723742480379562184628903067830946485102499067941935834976169248148345766508953066278488143098179385402295488972232456204464004698866491979082021708065979612455216709710029237202401744728024709940757140693227210826765663880836710099699802821571316573433951101749896869773760554823386485432388138304389456364244399112900391039771347122716036879496282871688668349472311822797231519507614822068307375172370530896941123923954739043564289242127195229719604596957031061498186581617793991013046547815324034982562741195477652010964731871833843101287056806480718363439469089518232947929463078838772443036546598148542488466851814918989038547005596200546751839751977317875711137113213326659705309410008721491805153910162044565649232928213650338417728333946014366760479133529367563176410907689113832128632852667922325801225987218166519002144201447724479700709643173059325620721185711001962559468057413215728795624565761853757057100821977970544164556723940825574893106924995821238384630009829976768719418882193141859435906525271887037939195284208719705089389052125504457822373955021685435429116976493023818555822558962084333254617359157583843308988927840111142364592236471161774231431923574976896724646748139060694413058568969024304895870082352272392651421124867924902440748273905974022226183227903011727192408033507864759888334119593384456467432431521562586146988621666632791906406258976842925018456527599500805963180044431835063675006595323426763684269243039015300645377512929896921915441470079808163316201484963551791621089880";
static const std::size_t DES_KEY_LENTH = 24;

class RegisterPrivate
{
    friend class Register;
    RSA *rsa;
    std::string datPath;
    RegisterPrivate()
        : rsa(NULL)
        , datPath()
    {

    }

    std::vector<std::string> decrypt()
    {
        std::ifstream in;

        in.open(datPath.c_str(), std::ios::in);

        if (in.fail())
            return std::vector<std::string>();

        std::string fileText((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        std::size_t count = cyoBase64DecodeGetLength(fileText.size());
        boost::scoped_array<char> cipher(new char[count]);
        count = cyoBase64Decode(cipher.get(), fileText.c_str(), fileText.size());

        if (cipher.get() == NULL || count <= 0)
        {
            return std::vector<std::string>();
        }

        int flen = RSA_size(rsa);
        boost::scoped_array<char> recovered(new char[flen]);
        memset(recovered.get(), 0, flen);

        int ret = RSA_private_decrypt(count, (const unsigned char*)cipher.get(), (unsigned char*)recovered.get(), rsa, RSA_PKCS1_OAEP_PADDING);
        if (ret < 0)
        {
            return std::vector<std::string>();
        }

        std::vector<std::string> result;
        std::string temp(recovered.get());
        boost::algorithm::split(result, temp, boost::is_any_of("\n"));

        if (result.size() < Register::MESSAGE_LINES )
        {
            return std::vector<std::string>();
        }

        return result;
    }
};


Register& Register::inst()
{
    static Register instance;
    return instance;
}

void Register::setPath(const std::string &path)
{
    _priv->datPath = path;
    _priv->datPath += "demo.dat";
}

void Register::saveReg(const std::string &reg)
{
    std::ofstream out;
    out.open(_priv->datPath.c_str(), std::ios::trunc);

    out << reg;
}

std::string Register::queryIDCode()
{
    std::string idCode;
    boost::scoped_array<char> result;
    std::stringstream plain;
    std::string temp;

    std::size_t count = DiskInfo::inst().load();
    std::size_t i;

    for (i = 0; i < count; i++)
    {
        temp = DiskInfo::inst().serialNumber(i);
        plain << temp << std::endl;
    }

    count = cyoBase16EncodeGetLength(plain.str().size());
    result.reset(new char[count]);

    cyoBase16Encode(result.get(), plain.str().c_str(), plain.str().size());

    idCode = result.get();
    idCode.resize(16);

    return idCode;
}

bool Register::checkReg()
{
    std::vector<std::string> vec = _priv->decrypt();
    if (vec.empty())
        return false;

    if (vec.size() < MESSAGE_LINES)
        return false;
    std::string idCode1 = vec[ID_CODE];
    std::string idCode2 = queryIDCode();

    return idCode1.compare(idCode2) == 0;
}

std::string Register::encryptLogText(const std::string &log)
{
    std::string logPW = decrypt(LOG_PW);
    if (logPW.empty())
    {
        return std::string();
    }
    else
    {
        return log;
    }
}

std::string Register::decrypt(DecryptType index)
{
    std::vector<std::string> vec = _priv->decrypt();
    if (vec.empty())
    {
        return std::string();
    }

    if (static_cast<std::size_t>(index) >= vec.size())
    {
        return std::string();
    }

    return vec[index];
}

Register::Register()
    : _priv(new RegisterPrivate())
{
    std::string n;
    std::string e;
    std::string d;

    long i;
    long length;

    length = strlen(_theN);
    for (i = 0; i < length; i += 2)
    {
        n += _theN[i];
    }

    length = strlen(_theE);
    for (i = 0; i < length; i += 3)
    {
        e += _theE[i];
    }

    length = strlen(_theD);
    for (i = 0; i < length; i += 4)
    {
        d += _theD[i];
    }

    _priv->rsa = RSA_new();

    _priv->rsa->n = BN_new();
    _priv->rsa->e = BN_new();
    _priv->rsa->d = BN_new();

    BN_dec2bn(&(_priv->rsa->n), n.c_str());
    BN_dec2bn(&(_priv->rsa->e), e.c_str());
    BN_dec2bn(&(_priv->rsa->d), d.c_str());
}

Register::~Register()
{
    RSA_free(_priv->rsa);
}

}
}
