//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "core.h"

/*
 * This file contains code and data related to various CIE standards.
 *  - the CIE-1931 Standard Colorimetric Observer data
 *  - the CIE-1964 Standard Colorimetric Observer data
 *  - the CIEDE2000 metric
 */

namespace lol
{

float Color::DistanceCIEDE2000(vec3 lab1, vec3 lab2)
{
    float const pi = 3.141592653589793f;

    float const deg2rad = 6.28318530718f / 360.f;
    float const rad2deg = 360.f / 6.28318530718f;

    float C1 = length(lab1.yz);
    float C2 = length(lab2.yz);
    float C_ = 0.5f * (C1 + C2);

    float L1 = lab1.x;
    float L2 = lab2.x;
    float dLp = L2 - L1;
    float L_ = 0.5f * (L1 + L2);

    float tmp1 = pow(C_, 7.f);
    float tmp2 = 1.5f - 0.5f * sqrt(tmp1 / (tmp1 + pow(25.f, 7.f)));
    float ap1 = lab1.y * tmp2;
    float ap2 = lab2.y * tmp2;
    float Cp1 = sqrt(ap1 * ap1 + lab1.z * lab1.z);
    float Cp2 = sqrt(ap2 * ap2 + lab2.z * lab2.z);
    float dCp = Cp2 - Cp1;
    float Cp_ = 0.5f * (Cp1 + Cp2);

    float hp1 = fmod(atan2(lab1.z, ap1) + 2.f * pi, 2.f * pi);
    float hp2 = fmod(atan2(lab2.z, ap2) + 2.f * pi, 2.f * pi);
    float dhp; /* -pi .. pi */
    if (abs(hp1 - hp2) <= pi)
        dhp = hp2 - hp1;
    else if (hp2 <= hp1)
        dhp = hp2 - hp1 + 2.f * pi;
    else
        dhp = hp2 - hp1 - 2.f * pi;
    float dHp = 2.f * sqrt(Cp1 * Cp2) * sin(dhp / 2.f);
    float Hp_; /* half-angle 0 .. 360 */
    if (!(Cp1 * Cp2))
        Hp_ = hp1 + hp2;
    else if (abs(hp1 - hp2) > pi && hp1 + hp2 < 2.f * pi)
        Hp_ = 0.5f * (hp1 + hp2) + pi;
    else if (abs(hp1 - hp2) > 180.f)
        Hp_ = 0.5f * (hp1 + hp2) - pi;
    else
        Hp_ = 0.5f * (hp1 + hp2);

    float T = 1.f - 0.17f * cos(Hp_ - pi / 6.f)
                  + 0.24f * cos(2.f * Hp_)
                  + 0.32f * cos(3.f * Hp_ + pi / 30.f)
                  - 0.20f * cos(4.f * Hp_ - 0.35f * pi);
    float SL = 1.f + 0.015f * (L_ - 50) * (L_ - 50)
                            / sqrt(20.f + (L_ - 50) * (L_ - 50));
    float SC = 1.f + 0.045f * Cp_;
    float SH = 1.f + 0.015f * Cp_ * T;
    float RT = -2.f * sqrt(pow(Cp_, 7.f) / (pow(Cp_, 7.f) + pow(25.f, 7.f)))
                    * sin(60.f * deg2rad * exp(-pow((Hp_ * rad2deg - 275.f) / 25.f, 2.f)));

    dLp /= SL;
    dCp /= SC;
    dHp /= SH;

    return sqrt(dLp * dLp + dCp * dCp + dHp * dHp + RT * dCp * dHp);
}

/* CIE 1931 Standard Colorimetric Observer, aka. 2º Observer
 * for wavelengths 360 to 830. */
static float cie_1931_xyz[] =
{
    /* 360 — 399 nm */
    0.0001299f, 3.917e-06f, 0.0006061f, 0.0001458f, 4.394e-06f, 0.0006809f,
    0.0001638f, 4.930e-06f, 0.0007651f, 0.0001840f, 5.532e-06f, 0.0008600f,
    0.0002067f, 6.208e-06f, 0.0009666f, 0.0002321f, 6.965e-06f, 0.0010860f,
    0.0002607f, 7.813e-06f, 0.0012206f, 0.0002931f, 8.767e-06f, 0.0013727f,
    0.0003294f, 9.840e-06f, 0.0015436f, 0.0003699f, 1.104e-05f, 0.0017343f,
    0.0004149f, 1.239e-05f, 0.0019460f, 0.0004642f, 1.389e-05f, 0.0021778f,
    0.0005190f, 1.556e-05f, 0.0024358f, 0.0005819f, 1.744e-05f, 0.0027320f,
    0.0006552f, 1.958e-05f, 0.0030781f, 0.0007416f, 2.202e-05f, 0.0034860f,
    0.0008450f, 2.484e-05f, 0.0039752f, 0.0009645f, 2.804e-05f, 0.0045409f,
    0.0010949f, 3.153e-05f, 0.0051583f, 0.0012312f, 3.522e-05f, 0.0058029f,
    0.0013680f, 3.900e-05f, 0.0064500f, 0.0015021f, 4.283e-05f, 0.0070832f,
    0.0016423f, 4.691e-05f, 0.0077455f, 0.0018024f, 5.159e-05f, 0.0085012f,
    0.0019958f, 5.718e-05f, 0.0094145f, 0.0022360f, 6.400e-05f, 0.0105500f,
    0.0025354f, 7.234e-05f, 0.0119658f, 0.0028926f, 8.221e-05f, 0.0136559f,
    0.0033008f, 9.351e-05f, 0.0155881f, 0.0037532f, 0.0001061f, 0.0177302f,
    0.0042430f, 0.0001200f, 0.0200500f, 0.0047624f, 0.0001350f, 0.0225114f,
    0.0053300f, 0.0001515f, 0.0252029f, 0.0059787f, 0.0001702f, 0.0282797f,
    0.0067411f, 0.0001918f, 0.0318970f, 0.0076500f, 0.0002170f, 0.0362100f,
    0.0087514f, 0.0002469f, 0.0414377f, 0.0100289f, 0.0002812f, 0.0475037f,
    0.0114217f, 0.0003185f, 0.0541199f, 0.0128690f, 0.0003573f, 0.0609980f,
    /* 400 — 439 nm */
    0.0143100f, 0.0003960f, 0.0678500f, 0.0157044f, 0.0004337f, 0.0744863f,
    0.0171474f, 0.0004730f, 0.0813616f, 0.0187812f, 0.0005179f, 0.0891536f,
    0.0207480f, 0.0005722f, 0.0985405f, 0.0231900f, 0.0006400f, 0.1102000f,
    0.0262074f, 0.0007246f, 0.1246133f, 0.0297825f, 0.0008255f, 0.1417017f,
    0.0338809f, 0.0009412f, 0.1613035f, 0.0384682f, 0.0010699f, 0.1832568f,
    0.0435100f, 0.0012100f, 0.2074000f, 0.0489956f, 0.0013621f, 0.2336921f,
    0.0550226f, 0.0015308f, 0.2626114f, 0.0617188f, 0.0017204f, 0.2947746f,
    0.0692120f, 0.0019353f, 0.3307985f, 0.0776300f, 0.0021800f, 0.3713000f,
    0.0869581f, 0.0024548f, 0.4162091f, 0.0971767f, 0.0027640f, 0.4654642f,
    0.1084063f, 0.0031178f, 0.5196948f, 0.1207672f, 0.0035264f, 0.5795303f,
    0.1343800f, 0.0040000f, 0.6456000f, 0.1493582f, 0.0045462f, 0.7184838f,
    0.1653957f, 0.0051593f, 0.7967133f, 0.1819831f, 0.0058293f, 0.8778459f,
    0.1986110f, 0.0065462f, 0.9594390f, 0.2147700f, 0.0073000f, 1.0390501f,
    0.2301868f, 0.0080865f, 1.1153673f, 0.2448797f, 0.0089087f, 1.1884971f,
    0.2587773f, 0.0097677f, 1.2581233f, 0.2718079f, 0.0106644f, 1.3239296f,
    0.2839000f, 0.0116000f, 1.3856000f, 0.2949438f, 0.0125732f, 1.4426352f,
    0.3048965f, 0.0135827f, 1.4948035f, 0.3137873f, 0.0146297f, 1.5421903f,
    0.3216454f, 0.0157151f, 1.5848807f, 0.3285000f, 0.0168400f, 1.6229600f,
    0.3343513f, 0.0180074f, 1.6564048f, 0.3392101f, 0.0192145f, 1.6852959f,
    0.3431213f, 0.0204539f, 1.7098745f, 0.3461296f, 0.0217182f, 1.7303821f,
    /* 440 — 479 nm */
    0.3482800f, 0.0230000f, 1.7470600f, 0.3495999f, 0.0242946f, 1.7600446f,
    0.3501474f, 0.0256102f, 1.7696233f, 0.3500130f, 0.0269586f, 1.7762637f,
    0.3492870f, 0.0283513f, 1.7804334f, 0.3480600f, 0.0298000f, 1.7826000f,
    0.3463733f, 0.0313108f, 1.7829682f, 0.3442624f, 0.0328837f, 1.7816998f,
    0.3418088f, 0.0345211f, 1.7791982f, 0.3390941f, 0.0362257f, 1.7758671f,
    0.3362000f, 0.0380000f, 1.7721100f, 0.3331977f, 0.0398467f, 1.7682589f,
    0.3300411f, 0.0417680f, 1.7640390f, 0.3266357f, 0.0437660f, 1.7589438f,
    0.3228868f, 0.0458427f, 1.7524663f, 0.3187000f, 0.0480000f, 1.7441000f,
    0.3140251f, 0.0502437f, 1.7335595f, 0.3088840f, 0.0525730f, 1.7208581f,
    0.3032904f, 0.0549806f, 1.7059369f, 0.2972579f, 0.0574587f, 1.6887372f,
    0.2908000f, 0.0600000f, 1.6692000f, 0.2839701f, 0.0626020f, 1.6475287f,
    0.2767214f, 0.0652775f, 1.6234127f, 0.2689178f, 0.0680421f, 1.5960223f,
    0.2604227f, 0.0709111f, 1.5645280f, 0.2511000f, 0.0739000f, 1.5281000f,
    0.2408475f, 0.0770160f, 1.4861114f, 0.2298512f, 0.0802664f, 1.4395215f,
    0.2184072f, 0.0836668f, 1.3898799f, 0.2068115f, 0.0872328f, 1.3387362f,
    0.1953600f, 0.0909800f, 1.2876400f, 0.1842136f, 0.0949176f, 1.2374223f,
    0.1733273f, 0.0990458f, 1.1878243f, 0.1626881f, 0.1033674f, 1.1387611f,
    0.1522833f, 0.1078846f, 1.0901480f, 0.1421000f, 0.1126000f, 1.0419000f,
    0.1321786f, 0.1175320f, 0.9941976f, 0.1225696f, 0.1226744f, 0.9473473f,
    0.1132752f, 0.1279928f, 0.9014531f, 0.1042979f, 0.1334528f, 0.8566193f,
    /* 480 — 519 nm */
    0.0956400f, 0.1390200f, 0.8129501f, 0.0872996f, 0.1446764f, 0.7705173f,
    0.0793080f, 0.1504693f, 0.7294448f, 0.0717178f, 0.1564619f, 0.6899136f,
    0.0645810f, 0.1627177f, 0.6521049f, 0.0579500f, 0.1693000f, 0.6162000f,
    0.0518621f, 0.1762431f, 0.5823286f, 0.0462815f, 0.1835581f, 0.5504162f,
    0.0411509f, 0.1912735f, 0.5203376f, 0.0364128f, 0.1994180f, 0.4919673f,
    0.0320100f, 0.2080200f, 0.4651800f, 0.0279172f, 0.2171199f, 0.4399246f,
    0.0241444f, 0.2267345f, 0.4161836f, 0.0206870f, 0.2368571f, 0.3938822f,
    0.0175404f, 0.2474812f, 0.3729459f, 0.0147000f, 0.2586000f, 0.3533000f,
    0.0121618f, 0.2701849f, 0.3348578f, 0.0099200f, 0.2822939f, 0.3175521f,
    0.0079672f, 0.2950505f, 0.3013375f, 0.0062963f, 0.3085780f, 0.2861686f,
    0.0049000f, 0.3230000f, 0.2720000f, 0.0037772f, 0.3384021f, 0.2588171f,
    0.0029453f, 0.3546858f, 0.2464838f, 0.0024249f, 0.3716986f, 0.2347718f,
    0.0022363f, 0.3892875f, 0.2234533f, 0.0024000f, 0.4073000f, 0.2123000f,
    0.0029255f, 0.4256299f, 0.2011692f, 0.0038366f, 0.4443096f, 0.1901196f,
    0.0051748f, 0.4633944f, 0.1792254f, 0.0069821f, 0.4829395f, 0.1685608f,
    0.0093000f, 0.5030000f, 0.1582000f, 0.0121495f, 0.5235693f, 0.1481383f,
    0.0155359f, 0.5445120f, 0.1383758f, 0.0194775f, 0.5656900f, 0.1289942f,
    0.0239928f, 0.5869653f, 0.1200751f, 0.0291000f, 0.6082000f, 0.1117000f,
    0.0348149f, 0.6293456f, 0.1039048f, 0.0411202f, 0.6503068f, 0.0966675f,
    0.0479850f, 0.6708752f, 0.0899827f, 0.0553786f, 0.6908424f, 0.0838453f,
    /* 520 — 559 nm */
    0.0632700f, 0.7100000f, 0.0782500f, 0.0716350f, 0.7281852f, 0.0732090f,
    0.0804622f, 0.7454636f, 0.0686782f, 0.0897400f, 0.7619694f, 0.0645678f,
    0.0994565f, 0.7778368f, 0.0607884f, 0.1096000f, 0.7932000f, 0.0572500f,
    0.1201674f, 0.8081104f, 0.0539044f, 0.1311145f, 0.8224962f, 0.0507466f,
    0.1423679f, 0.8363068f, 0.0477528f, 0.1538542f, 0.8494916f, 0.0448986f,
    0.1655000f, 0.8620000f, 0.0421600f, 0.1772571f, 0.8738108f, 0.0395073f,
    0.1891400f, 0.8849624f, 0.0369356f, 0.2011694f, 0.8954936f, 0.0344584f,
    0.2133658f, 0.9054432f, 0.0320887f, 0.2257499f, 0.9148501f, 0.0298400f,
    0.2383209f, 0.9237348f, 0.0277118f, 0.2510668f, 0.9320924f, 0.0256944f,
    0.2639922f, 0.9399226f, 0.0237872f, 0.2771017f, 0.9472252f, 0.0219893f,
    0.2904000f, 0.9540000f, 0.0203000f, 0.3038912f, 0.9602561f, 0.0187181f,
    0.3175726f, 0.9660074f, 0.0172404f, 0.3314384f, 0.9712606f, 0.0158636f,
    0.3454828f, 0.9760225f, 0.0145846f, 0.3597000f, 0.9803000f, 0.0134000f,
    0.3740839f, 0.9840924f, 0.0123072f, 0.3886396f, 0.9874182f, 0.0113019f,
    0.4033784f, 0.9903128f, 0.0103779f, 0.4183115f, 0.9928116f, 0.0095293f,
    0.4334499f, 0.9949501f, 0.0087500f, 0.4487953f, 0.9967108f, 0.0080352f,
    0.4643360f, 0.9980983f, 0.0073816f, 0.4800640f, 0.9991120f, 0.0067854f,
    0.4959713f, 0.9997482f, 0.0062428f, 0.5120501f, 1.0000000f, 0.0057500f,
    0.5282959f, 0.9998567f, 0.0053036f, 0.5446916f, 0.9993046f, 0.0048998f,
    0.5612094f, 0.9983255f, 0.0045342f, 0.5778215f, 0.9968987f, 0.0042024f,
    /* 560 — 599 nm */
    0.5945000f, 0.9950000f, 0.0039000f, 0.6112209f, 0.9926005f, 0.0036232f,
    0.6279758f, 0.9897426f, 0.0033706f, 0.6447602f, 0.9864444f, 0.0031414f,
    0.6615697f, 0.9827241f, 0.0029348f, 0.6784000f, 0.9786000f, 0.0027500f,
    0.6952392f, 0.9740837f, 0.0025852f, 0.7120586f, 0.9691712f, 0.0024386f,
    0.7288284f, 0.9638568f, 0.0023094f, 0.7455188f, 0.9581349f, 0.0021968f,
    0.7621000f, 0.9520000f, 0.0021000f, 0.7785432f, 0.9454504f, 0.0020177f,
    0.7948256f, 0.9384992f, 0.0019482f, 0.8109264f, 0.9311628f, 0.0018898f,
    0.8268248f, 0.9234576f, 0.0018409f, 0.8425000f, 0.9154000f, 0.0018000f,
    0.8579325f, 0.9070064f, 0.0017663f, 0.8730816f, 0.8982772f, 0.0017378f,
    0.8878944f, 0.8892048f, 0.0017112f, 0.9023181f, 0.8797816f, 0.0016831f,
    0.9163000f, 0.8700000f, 0.0016500f, 0.9297995f, 0.8598613f, 0.0016101f,
    0.9427984f, 0.8493920f, 0.0015644f, 0.9552776f, 0.8386220f, 0.0015136f,
    0.9672179f, 0.8275813f, 0.0014585f, 0.9786000f, 0.8163000f, 0.0014000f,
    0.9893856f, 0.8047947f, 0.0013367f, 0.9995488f, 0.7930820f, 0.0012700f,
    1.0090892f, 0.7811920f, 0.0012050f, 1.0180064f, 0.7691547f, 0.0011467f,
    1.0263000f, 0.7570000f, 0.0011000f, 1.0339827f, 0.7447541f, 0.0010688f,
    1.0409860f, 0.7324224f, 0.0010494f, 1.0471880f, 0.7200036f, 0.0010356f,
    1.0524667f, 0.7074965f, 0.0010212f, 1.0567000f, 0.6949000f, 0.0010000f,
    1.0597944f, 0.6822192f, 0.0009686f, 1.0617992f, 0.6694716f, 0.0009299f,
    1.0628068f, 0.6566744f, 0.0008869f, 1.0629096f, 0.6438448f, 0.0008426f,
    /* 600 — 639 nm */
    1.0622000f, 0.6310000f, 0.0008000f, 1.0607352f, 0.6181555f, 0.0007610f,
    1.0584436f, 0.6053144f, 0.0007237f, 1.0552244f, 0.5924756f, 0.0006859f,
    1.0509768f, 0.5796379f, 0.0006454f, 1.0456000f, 0.5668000f, 0.0006000f,
    1.0390369f, 0.5539611f, 0.0005479f, 1.0313608f, 0.5411372f, 0.0004916f,
    1.0226662f, 0.5283528f, 0.0004354f, 1.0130477f, 0.5156323f, 0.0003835f,
    1.0026000f, 0.5030000f, 0.0003400f, 0.9913675f, 0.4904688f, 0.0003073f,
    0.9793314f, 0.4780304f, 0.0002832f, 0.9664916f, 0.4656776f, 0.0002654f,
    0.9528479f, 0.4534032f, 0.0002518f, 0.9384000f, 0.4412000f, 0.0002400f,
    0.9231940f, 0.4290800f, 0.0002295f, 0.9072440f, 0.4170360f, 0.0002206f,
    0.8905020f, 0.4050320f, 0.0002120f, 0.8729200f, 0.3930320f, 0.0002022f,
    0.8544499f, 0.3810000f, 0.0001900f, 0.8350840f, 0.3689184f, 0.0001742f,
    0.8149460f, 0.3568272f, 0.0001556f, 0.7941860f, 0.3447768f, 0.0001360f,
    0.7729540f, 0.3328176f, 0.0001169f, 0.7514000f, 0.3210000f, 0.0001000f,
    0.7295836f, 0.3093381f, 8.613e-05f, 0.7075888f, 0.2978504f, 7.460e-05f,
    0.6856022f, 0.2865936f, 6.500e-05f, 0.6638104f, 0.2756245f, 5.693e-05f,
    0.6424000f, 0.2650000f, 5.000e-05f, 0.6215149f, 0.2547632f, 4.416e-05f,
    0.6011138f, 0.2448896f, 3.948e-05f, 0.5811052f, 0.2353344f, 3.572e-05f,
    0.5613977f, 0.2260528f, 3.264e-05f, 0.5419000f, 0.2170000f, 3.000e-05f,
    0.5225995f, 0.2081616f, 2.765e-05f, 0.5035464f, 0.1995488f, 2.556e-05f,
    0.4847436f, 0.1911552f, 2.364e-05f, 0.4661939f, 0.1829744f, 2.181e-05f,
    /* 640 — 679 nm */
    0.4479000f, 0.1750000f, 2.000e-05f, 0.4298613f, 0.1672235f, 1.813e-05f,
    0.4120980f, 0.1596464f, 1.620e-05f, 0.3946440f, 0.1522776f, 1.420e-05f,
    0.3775333f, 0.1451259f, 1.213e-05f, 0.3608000f, 0.1382000f, 1.000e-05f,
    0.3444563f, 0.1315003f, 7.733e-06f, 0.3285168f, 0.1250248f, 5.400e-06f,
    0.3130192f, 0.1187792f, 3.200e-06f, 0.2980011f, 0.1127691f, 1.333e-06f,
    0.2835000f, 0.1070000f, 0.0000000f, 0.2695448f, 0.1014762f, 0.0000000f,
    0.2561184f, 0.0961886f, 0.0000000f, 0.2431896f, 0.0911230f, 0.0000000f,
    0.2307272f, 0.0862649f, 0.0000000f, 0.2187000f, 0.0816000f, 0.0000000f,
    0.2070971f, 0.0771206f, 0.0000000f, 0.1959232f, 0.0728255f, 0.0000000f,
    0.1851708f, 0.0687101f, 0.0000000f, 0.1748323f, 0.0647698f, 0.0000000f,
    0.1649000f, 0.0610000f, 0.0000000f, 0.1553667f, 0.0573962f, 0.0000000f,
    0.1462300f, 0.0539550f, 0.0000000f, 0.1374900f, 0.0506738f, 0.0000000f,
    0.1291467f, 0.0475497f, 0.0000000f, 0.1212000f, 0.0445800f, 0.0000000f,
    0.1136397f, 0.0417587f, 0.0000000f, 0.1064650f, 0.0390850f, 0.0000000f,
    0.0996904f, 0.0365638f, 0.0000000f, 0.0933306f, 0.0342005f, 0.0000000f,
    0.0874000f, 0.0320000f, 0.0000000f, 0.0819010f, 0.0299626f, 0.0000000f,
    0.0768043f, 0.0280766f, 0.0000000f, 0.0720771f, 0.0263294f, 0.0000000f,
    0.0676866f, 0.0247081f, 0.0000000f, 0.0636000f, 0.0232000f, 0.0000000f,
    0.0598069f, 0.0218008f, 0.0000000f, 0.0562822f, 0.0205011f, 0.0000000f,
    0.0529710f, 0.0192811f, 0.0000000f, 0.0498186f, 0.0181207f, 0.0000000f,
    /* 680 — 719 nm */
    0.0467700f, 0.0170000f, 0.0000000f, 0.0437841f, 0.0159038f, 0.0000000f,
    0.0408754f, 0.0148372f, 0.0000000f, 0.0380726f, 0.0138107f, 0.0000000f,
    0.0354046f, 0.0128348f, 0.0000000f, 0.0329000f, 0.0119200f, 0.0000000f,
    0.0305642f, 0.0110683f, 0.0000000f, 0.0283806f, 0.0102734f, 0.0000000f,
    0.0263448f, 0.0095333f, 0.0000000f, 0.0244528f, 0.0088462f, 0.0000000f,
    0.0227000f, 0.0082100f, 0.0000000f, 0.0210843f, 0.0076238f, 0.0000000f,
    0.0195999f, 0.0070854f, 0.0000000f, 0.0182373f, 0.0065915f, 0.0000000f,
    0.0169872f, 0.0061385f, 0.0000000f, 0.0158400f, 0.0057230f, 0.0000000f,
    0.0147906f, 0.0053431f, 0.0000000f, 0.0138313f, 0.0049958f, 0.0000000f,
    0.0129487f, 0.0046764f, 0.0000000f, 0.0121292f, 0.0043801f, 0.0000000f,
    0.0113592f, 0.0041020f, 0.0000000f, 0.0106294f, 0.0038385f, 0.0000000f,
    0.0099388f, 0.0035891f, 0.0000000f, 0.0092884f, 0.0033542f, 0.0000000f,
    0.0086789f, 0.0031341f, 0.0000000f, 0.0081109f, 0.0029290f, 0.0000000f,
    0.0075824f, 0.0027381f, 0.0000000f, 0.0070887f, 0.0025599f, 0.0000000f,
    0.0066273f, 0.0023932f, 0.0000000f, 0.0061954f, 0.0022373f, 0.0000000f,
    0.0057903f, 0.0020910f, 0.0000000f, 0.0054098f, 0.0019536f, 0.0000000f,
    0.0050526f, 0.0018246f, 0.0000000f, 0.0047175f, 0.0017036f, 0.0000000f,
    0.0044035f, 0.0015902f, 0.0000000f, 0.0041095f, 0.0014840f, 0.0000000f,
    0.0038339f, 0.0013845f, 0.0000000f, 0.0035757f, 0.0012913f, 0.0000000f,
    0.0033343f, 0.0012041f, 0.0000000f, 0.0031091f, 0.0011227f, 0.0000000f,
    /* 720 — 759 nm */
    0.0028993f, 0.0010470f, 0.0000000f, 0.0027043f, 0.0009766f, 0.0000000f,
    0.0025230f, 0.0009111f, 0.0000000f, 0.0023542f, 0.0008501f, 0.0000000f,
    0.0021966f, 0.0007932f, 0.0000000f, 0.0020492f, 0.0007400f, 0.0000000f,
    0.0019110f, 0.0006901f, 0.0000000f, 0.0017814f, 0.0006433f, 0.0000000f,
    0.0016601f, 0.0005995f, 0.0000000f, 0.0015465f, 0.0005585f, 0.0000000f,
    0.0014400f, 0.0005200f, 0.0000000f, 0.0013400f, 0.0004839f, 0.0000000f,
    0.0012463f, 0.0004501f, 0.0000000f, 0.0011585f, 0.0004183f, 0.0000000f,
    0.0010764f, 0.0003887f, 0.0000000f, 0.0009999f, 0.0003611f, 0.0000000f,
    0.0009287f, 0.0003354f, 0.0000000f, 0.0008624f, 0.0003114f, 0.0000000f,
    0.0008008f, 0.0002892f, 0.0000000f, 0.0007434f, 0.0002685f, 0.0000000f,
    0.0006901f, 0.0002492f, 0.0000000f, 0.0006405f, 0.0002313f, 0.0000000f,
    0.0005945f, 0.0002147f, 0.0000000f, 0.0005519f, 0.0001993f, 0.0000000f,
    0.0005124f, 0.0001850f, 0.0000000f, 0.0004760f, 0.0001719f, 0.0000000f,
    0.0004425f, 0.0001598f, 0.0000000f, 0.0004115f, 0.0001486f, 0.0000000f,
    0.0003830f, 0.0001383f, 0.0000000f, 0.0003566f, 0.0001288f, 0.0000000f,
    0.0003323f, 0.0001200f, 0.0000000f, 0.0003098f, 0.0001119f, 0.0000000f,
    0.0002889f, 0.0001043f, 0.0000000f, 0.0002695f, 9.734e-05f, 0.0000000f,
    0.0002516f, 9.085e-05f, 0.0000000f, 0.0002348f, 8.480e-05f, 0.0000000f,
    0.0002192f, 7.915e-05f, 0.0000000f, 0.0002045f, 7.386e-05f, 0.0000000f,
    0.0001908f, 6.892e-05f, 0.0000000f, 0.0001781f, 6.430e-05f, 0.0000000f,
    /* 760 — 799 nm */
    0.0001662f, 6.000e-05f, 0.0000000f, 0.0001550f, 5.598e-05f, 0.0000000f,
    0.0001446f, 5.223e-05f, 0.0000000f, 0.0001349f, 4.872e-05f, 0.0000000f,
    0.0001259f, 4.545e-05f, 0.0000000f, 0.0001174f, 4.240e-05f, 0.0000000f,
    0.0001096f, 3.956e-05f, 0.0000000f, 0.0001022f, 3.692e-05f, 0.0000000f,
    9.539e-05f, 3.445e-05f, 0.0000000f, 8.902e-05f, 3.215e-05f, 0.0000000f,
    8.308e-05f, 3.000e-05f, 0.0000000f, 7.751e-05f, 2.799e-05f, 0.0000000f,
    7.231e-05f, 2.611e-05f, 0.0000000f, 6.746e-05f, 2.436e-05f, 0.0000000f,
    6.293e-05f, 2.272e-05f, 0.0000000f, 5.871e-05f, 2.120e-05f, 0.0000000f,
    5.477e-05f, 1.978e-05f, 0.0000000f, 5.110e-05f, 1.845e-05f, 0.0000000f,
    4.768e-05f, 1.722e-05f, 0.0000000f, 4.449e-05f, 1.606e-05f, 0.0000000f,
    4.151e-05f, 1.499e-05f, 0.0000000f, 3.873e-05f, 1.399e-05f, 0.0000000f,
    3.614e-05f, 1.305e-05f, 0.0000000f, 3.372e-05f, 1.218e-05f, 0.0000000f,
    3.146e-05f, 1.136e-05f, 0.0000000f, 2.935e-05f, 1.060e-05f, 0.0000000f,
    2.738e-05f, 9.886e-06f, 0.0000000f, 2.552e-05f, 9.217e-06f, 0.0000000f,
    2.379e-05f, 8.592e-06f, 0.0000000f, 2.218e-05f, 8.009e-06f, 0.0000000f,
    2.067e-05f, 7.466e-06f, 0.0000000f, 1.927e-05f, 6.960e-06f, 0.0000000f,
    1.797e-05f, 6.488e-06f, 0.0000000f, 1.675e-05f, 6.049e-06f, 0.0000000f,
    1.562e-05f, 5.639e-06f, 0.0000000f, 1.456e-05f, 5.258e-06f, 0.0000000f,
    1.357e-05f, 4.902e-06f, 0.0000000f, 1.265e-05f, 4.570e-06f, 0.0000000f,
    1.180e-05f, 4.260e-06f, 0.0000000f, 1.100e-05f, 3.972e-06f, 0.0000000f,
    /* 800 — 830 nm */
    1.025e-05f, 3.703e-06f, 0.0000000f, 9.560e-06f, 3.452e-06f, 0.0000000f,
    8.912e-06f, 3.218e-06f, 0.0000000f, 8.308e-06f, 3.000e-06f, 0.0000000f,
    7.746e-06f, 2.797e-06f, 0.0000000f, 7.221e-06f, 2.608e-06f, 0.0000000f,
    6.732e-06f, 2.431e-06f, 0.0000000f, 6.276e-06f, 2.267e-06f, 0.0000000f,
    5.851e-06f, 2.113e-06f, 0.0000000f, 5.455e-06f, 1.970e-06f, 0.0000000f,
    5.086e-06f, 1.837e-06f, 0.0000000f, 4.741e-06f, 1.712e-06f, 0.0000000f,
    4.420e-06f, 1.596e-06f, 0.0000000f, 4.121e-06f, 1.488e-06f, 0.0000000f,
    3.842e-06f, 1.387e-06f, 0.0000000f, 3.582e-06f, 1.293e-06f, 0.0000000f,
    3.339e-06f, 1.206e-06f, 0.0000000f, 3.113e-06f, 1.124e-06f, 0.0000000f,
    2.902e-06f, 1.048e-06f, 0.0000000f, 2.706e-06f, 9.771e-07f, 0.0000000f,
    2.523e-06f, 9.109e-07f, 0.0000000f, 2.352e-06f, 8.493e-07f, 0.0000000f,
    2.192e-06f, 7.917e-07f, 0.0000000f, 2.044e-06f, 7.381e-07f, 0.0000000f,
    1.905e-06f, 6.881e-07f, 0.0000000f, 1.777e-06f, 6.415e-07f, 0.0000000f,
    1.656e-06f, 5.981e-07f, 0.0000000f, 1.544e-06f, 5.576e-07f, 0.0000000f,
    1.439e-06f, 5.198e-07f, 0.0000000f, 1.342e-06f, 4.846e-07f, 0.0000000f,
    1.251e-06f, 4.518e-07f, 0.0000000f,
};

/* CIE 1964 Colorimetric Observer, aka. 10º Observer
 * for wavelengths 360 to 830. */
static float cie_1964_xyz[] =
{
    /* 360 — 399 nm */
    1.222e-07f, 1.340e-08f, 5.350e-07f, 1.851e-07f, 2.029e-08f, 8.107e-07f,
    2.788e-07f, 3.056e-08f, 1.221e-06f, 4.175e-07f, 4.574e-08f, 1.829e-06f,
    6.213e-07f, 6.805e-08f, 2.722e-06f, 9.193e-07f, 1.007e-07f, 4.028e-06f,
    1.352e-06f, 1.480e-07f, 5.926e-06f, 1.977e-06f, 2.163e-07f, 8.665e-06f,
    2.873e-06f, 3.142e-07f, 1.260e-05f, 4.150e-06f, 4.537e-07f, 1.820e-05f,
    5.959e-06f, 6.511e-07f, 2.614e-05f, 8.506e-06f, 9.288e-07f, 3.733e-05f,
    1.207e-05f, 1.318e-06f, 5.299e-05f, 1.702e-05f, 1.857e-06f, 7.476e-05f,
    2.387e-05f, 2.602e-06f, 0.0001049f, 3.327e-05f, 3.625e-06f, 0.0001462f,
    4.609e-05f, 5.019e-06f, 0.0002027f, 6.347e-05f, 6.907e-06f, 0.0002792f,
    8.689e-05f, 9.449e-06f, 0.0003825f, 0.0001182f, 1.285e-05f, 0.0005207f,
    0.0001600f, 1.736e-05f, 0.0007048f, 0.0002151f, 2.333e-05f, 0.0009482f,
    0.0002875f, 3.115e-05f, 0.0012682f, 0.0003820f, 4.135e-05f, 0.0016861f,
    0.0005046f, 5.456e-05f, 0.0022285f, 0.0006624f, 7.156e-05f, 0.0029278f,
    0.0008645f, 0.0000933f, 0.0038237f, 0.0011215f, 0.0001209f, 0.0049642f,
    0.0014462f, 0.0001556f, 0.0064067f, 0.0018536f, 0.0001992f, 0.0082193f,
    0.0023616f, 0.0002534f, 0.0104822f, 0.0029906f, 0.0003202f, 0.0132890f,
    0.0037645f, 0.0004024f, 0.0167470f, 0.0047102f, 0.0005023f, 0.0209800f,
    0.0058581f, 0.0006232f, 0.0261270f, 0.0072423f, 0.0007685f, 0.0323440f,
    0.0088996f, 0.0009417f, 0.0398020f, 0.0108709f, 0.0011478f, 0.0486910f,
    0.0131989f, 0.0013903f, 0.0592100f, 0.0159292f, 0.0016740f, 0.0715760f,
    /* 400 — 439 nm */
    0.0191097f, 0.0020044f, 0.0860109f, 0.0227880f, 0.0023860f, 0.1027400f,
    0.0270110f, 0.0028220f, 0.1220000f, 0.0318290f, 0.0033190f, 0.1440200f,
    0.0372780f, 0.0038800f, 0.1689900f, 0.0434000f, 0.0045090f, 0.1971200f,
    0.0502230f, 0.0052090f, 0.2285700f, 0.0577640f, 0.0059850f, 0.2634700f,
    0.0660380f, 0.0068330f, 0.3019000f, 0.0750330f, 0.0077570f, 0.3438700f,
    0.0847360f, 0.0087560f, 0.3893660f, 0.0950410f, 0.0098160f, 0.4379700f,
    0.1058360f, 0.0109180f, 0.4892200f, 0.1170660f, 0.0120580f, 0.5429000f,
    0.1286820f, 0.0132370f, 0.5988100f, 0.1406380f, 0.0144560f, 0.6567600f,
    0.1528930f, 0.0157170f, 0.7165800f, 0.1654160f, 0.0170250f, 0.7781200f,
    0.1781910f, 0.0183990f, 0.8413100f, 0.1912140f, 0.0198480f, 0.9061100f,
    0.2044920f, 0.0213910f, 0.9725420f, 0.2176500f, 0.0229920f, 1.0389000f,
    0.2302670f, 0.0245980f, 1.1031000f, 0.2423110f, 0.0262130f, 1.1651000f,
    0.2537930f, 0.0278410f, 1.2249000f, 0.2647370f, 0.0294970f, 1.2825000f,
    0.2751950f, 0.0311950f, 1.3382000f, 0.2853010f, 0.0329270f, 1.3926000f,
    0.2951430f, 0.0347380f, 1.4461000f, 0.3048690f, 0.0366540f, 1.4994000f,
    0.3146790f, 0.0386760f, 1.5534800f, 0.3243550f, 0.0407920f, 1.6072000f,
    0.3335700f, 0.0429460f, 1.6589000f, 0.3422430f, 0.0451140f, 1.7082000f,
    0.3503120f, 0.0473330f, 1.7548000f, 0.3577190f, 0.0496020f, 1.7985000f,
    0.3644820f, 0.0519340f, 1.8392000f, 0.3704930f, 0.0543370f, 1.8766000f,
    0.3757270f, 0.0568220f, 1.9105000f, 0.3801580f, 0.0593990f, 1.9408000f,
    /* 440 — 479 nm */
    0.3837340f, 0.0620770f, 1.9672800f, 0.3863270f, 0.0647370f, 1.9891000f,
    0.3878580f, 0.0672850f, 2.0057000f, 0.3883960f, 0.0697640f, 2.0174000f,
    0.3879780f, 0.0722180f, 2.0244000f, 0.3867260f, 0.0747040f, 2.0273000f,
    0.3846960f, 0.0772720f, 2.0264000f, 0.3820060f, 0.0799790f, 2.0223000f,
    0.3787090f, 0.0828740f, 2.0153000f, 0.3749150f, 0.0860000f, 2.0060000f,
    0.3707020f, 0.0894560f, 1.9948000f, 0.3660890f, 0.0929470f, 1.9814000f,
    0.3610450f, 0.0962750f, 1.9653000f, 0.3555180f, 0.0995350f, 1.9464000f,
    0.3494860f, 0.1028290f, 1.9248000f, 0.3429570f, 0.1062560f, 1.9007000f,
    0.3358930f, 0.1099010f, 1.8741000f, 0.3282840f, 0.1138350f, 1.8451000f,
    0.3201500f, 0.1181670f, 1.8139000f, 0.3114750f, 0.1229320f, 1.7806000f,
    0.3022730f, 0.1282010f, 1.7453700f, 0.2928580f, 0.1334570f, 1.7091000f,
    0.2835020f, 0.1383230f, 1.6723000f, 0.2740440f, 0.1430420f, 1.6347000f,
    0.2642630f, 0.1477870f, 1.5956000f, 0.2540850f, 0.1527610f, 1.5549000f,
    0.2433920f, 0.1581020f, 1.5122000f, 0.2321870f, 0.1639410f, 1.4673000f,
    0.2204880f, 0.1703620f, 1.4199000f, 0.2081980f, 0.1774250f, 1.3700000f,
    0.1956180f, 0.1851900f, 1.3175600f, 0.1830340f, 0.1930250f, 1.2624000f,
    0.1702220f, 0.2003130f, 1.2050000f, 0.1573480f, 0.2071560f, 1.1466000f,
    0.1446500f, 0.2136440f, 1.0880000f, 0.1323490f, 0.2199400f, 1.0302000f,
    0.1205840f, 0.2261700f, 0.9738300f, 0.1094560f, 0.2324670f, 0.9194300f,
    0.0990420f, 0.2390250f, 0.8674600f, 0.0893880f, 0.2459970f, 0.8182800f,
    /* 480 — 519 nm */
    0.0805070f, 0.2535890f, 0.7721250f, 0.0720340f, 0.2618760f, 0.7282900f,
    0.0637100f, 0.2706430f, 0.6860400f, 0.0556940f, 0.2796450f, 0.6455300f,
    0.0481170f, 0.2886940f, 0.6068500f, 0.0410720f, 0.2976650f, 0.5700600f,
    0.0346420f, 0.3064690f, 0.5352200f, 0.0288960f, 0.3150350f, 0.5023400f,
    0.0238760f, 0.3233350f, 0.4714000f, 0.0196280f, 0.3313660f, 0.4423900f,
    0.0161720f, 0.3391330f, 0.4152540f, 0.0133000f, 0.3478600f, 0.3900240f,
    0.0107590f, 0.3583260f, 0.3663990f, 0.0085420f, 0.3700010f, 0.3440150f,
    0.0066610f, 0.3824640f, 0.3226890f, 0.0051320f, 0.3953790f, 0.3023560f,
    0.0039820f, 0.4084820f, 0.2830360f, 0.0032390f, 0.4215880f, 0.2648160f,
    0.0029340f, 0.4346190f, 0.2478480f, 0.0031140f, 0.4476010f, 0.2323180f,
    0.0038160f, 0.4607770f, 0.2185020f, 0.0050950f, 0.4743400f, 0.2058510f,
    0.0069360f, 0.4882000f, 0.1935960f, 0.0092990f, 0.5023400f, 0.1817360f,
    0.0121470f, 0.5167400f, 0.1702810f, 0.0154440f, 0.5313600f, 0.1592490f,
    0.0191560f, 0.5461900f, 0.1486730f, 0.0232500f, 0.5611800f, 0.1386090f,
    0.0276900f, 0.5762900f, 0.1290960f, 0.0324440f, 0.5915000f, 0.1202150f,
    0.0374650f, 0.6067410f, 0.1120440f, 0.0429560f, 0.6221500f, 0.1047100f,
    0.0491140f, 0.6378300f, 0.0981960f, 0.0559200f, 0.6537100f, 0.0923610f,
    0.0633490f, 0.6696800f, 0.0870880f, 0.0713580f, 0.6856600f, 0.0822480f,
    0.0799010f, 0.7015500f, 0.0777440f, 0.0889090f, 0.7172300f, 0.0734560f,
    0.0982930f, 0.7325700f, 0.0692680f, 0.1079490f, 0.7474600f, 0.0650600f,
    /* 520 — 559 nm */
    0.1177490f, 0.7617570f, 0.0607090f, 0.1278390f, 0.7753400f, 0.0564570f,
    0.1384500f, 0.7882200f, 0.0526090f, 0.1495160f, 0.8004600f, 0.0491220f,
    0.1610410f, 0.8121400f, 0.0459540f, 0.1729530f, 0.8233300f, 0.0430500f,
    0.1852090f, 0.8341200f, 0.0403680f, 0.1977550f, 0.8446000f, 0.0378390f,
    0.2105380f, 0.8548700f, 0.0353840f, 0.2234600f, 0.8650400f, 0.0329490f,
    0.2364910f, 0.8752110f, 0.0304510f, 0.2496330f, 0.8853700f, 0.0280290f,
    0.2629720f, 0.8953700f, 0.0258620f, 0.2765150f, 0.9051500f, 0.0239200f,
    0.2902690f, 0.9146500f, 0.0221740f, 0.3042130f, 0.9238100f, 0.0205840f,
    0.3183610f, 0.9325500f, 0.0191270f, 0.3327050f, 0.9408100f, 0.0177400f,
    0.3472320f, 0.9485200f, 0.0164030f, 0.3619260f, 0.9556000f, 0.0150640f,
    0.3767720f, 0.9619880f, 0.0136760f, 0.3916830f, 0.9675400f, 0.0123080f,
    0.4065940f, 0.9722300f, 0.0110560f, 0.4215390f, 0.9761700f, 0.0099150f,
    0.4365170f, 0.9794600f, 0.0088720f, 0.4515840f, 0.9822000f, 0.0079180f,
    0.4667820f, 0.9845200f, 0.0070300f, 0.4821470f, 0.9865200f, 0.0062230f,
    0.4977380f, 0.9883200f, 0.0054530f, 0.5136060f, 0.9900200f, 0.0047140f,
    0.5298260f, 0.9917610f, 0.0039880f, 0.5464400f, 0.9935300f, 0.0032890f,
    0.5634260f, 0.9952300f, 0.0026460f, 0.5807260f, 0.9967700f, 0.0020630f,
    0.5982900f, 0.9980900f, 0.0015330f, 0.6160530f, 0.9991100f, 0.0010910f,
    0.6339480f, 0.9997700f, 0.0007110f, 0.6519010f, 1.0000000f, 0.0004070f,
    0.6698240f, 0.9997100f, 0.0001840f, 0.6876320f, 0.9988500f, 0.0000470f,
    /* 560 — 599 nm */
    0.7052240f, 0.9973400f, 0.0000000f, 0.7227730f, 0.9952600f, 0.0000000f,
    0.7404830f, 0.9927400f, 0.0000000f, 0.7582730f, 0.9897500f, 0.0000000f,
    0.7760830f, 0.9863000f, 0.0000000f, 0.7938320f, 0.9823800f, 0.0000000f,
    0.8114360f, 0.9779800f, 0.0000000f, 0.8288220f, 0.9731100f, 0.0000000f,
    0.8458790f, 0.9677400f, 0.0000000f, 0.8625250f, 0.9618900f, 0.0000000f,
    0.8786550f, 0.9555520f, 0.0000000f, 0.8942080f, 0.9486010f, 0.0000000f,
    0.9092060f, 0.9409810f, 0.0000000f, 0.9236720f, 0.9327980f, 0.0000000f,
    0.9376380f, 0.9241580f, 0.0000000f, 0.9511620f, 0.9151750f, 0.0000000f,
    0.9642830f, 0.9059540f, 0.0000000f, 0.9770680f, 0.8966080f, 0.0000000f,
    0.9895900f, 0.8872490f, 0.0000000f, 1.0019100f, 0.8779860f, 0.0000000f,
    1.0141600f, 0.8689340f, 0.0000000f, 1.0265000f, 0.8601640f, 0.0000000f,
    1.0388000f, 0.8515190f, 0.0000000f, 1.0510000f, 0.8429630f, 0.0000000f,
    1.0629000f, 0.8343930f, 0.0000000f, 1.0743000f, 0.8256230f, 0.0000000f,
    1.0852000f, 0.8167640f, 0.0000000f, 1.0952000f, 0.8075440f, 0.0000000f,
    1.1042000f, 0.7979470f, 0.0000000f, 1.1120000f, 0.7878930f, 0.0000000f,
    1.1185200f, 0.7774050f, 0.0000000f, 1.1238000f, 0.7664900f, 0.0000000f,
    1.1280000f, 0.7553090f, 0.0000000f, 1.1311000f, 0.7438450f, 0.0000000f,
    1.1332000f, 0.7321900f, 0.0000000f, 1.1343000f, 0.7203530f, 0.0000000f,
    1.1343000f, 0.7082810f, 0.0000000f, 1.1333000f, 0.6960550f, 0.0000000f,
    1.1312000f, 0.6836210f, 0.0000000f, 1.1281000f, 0.6710480f, 0.0000000f,
    /* 600 — 639 nm */
    1.1239900f, 0.6583410f, 0.0000000f, 1.1189000f, 0.6455450f, 0.0000000f,
    1.1129000f, 0.6327180f, 0.0000000f, 1.1059000f, 0.6198150f, 0.0000000f,
    1.0980000f, 0.6068870f, 0.0000000f, 1.0891000f, 0.5938780f, 0.0000000f,
    1.0792000f, 0.5807810f, 0.0000000f, 1.0684000f, 0.5676530f, 0.0000000f,
    1.0567000f, 0.5544900f, 0.0000000f, 1.0440000f, 0.5412280f, 0.0000000f,
    1.0304800f, 0.5279630f, 0.0000000f, 1.0160000f, 0.5146340f, 0.0000000f,
    1.0008000f, 0.5013630f, 0.0000000f, 0.9847900f, 0.4881240f, 0.0000000f,
    0.9680800f, 0.4749350f, 0.0000000f, 0.9507400f, 0.4618340f, 0.0000000f,
    0.9328000f, 0.4488230f, 0.0000000f, 0.9143400f, 0.4359170f, 0.0000000f,
    0.8953900f, 0.4231530f, 0.0000000f, 0.8760300f, 0.4105260f, 0.0000000f,
    0.8562970f, 0.3980570f, 0.0000000f, 0.8363500f, 0.3858350f, 0.0000000f,
    0.8162900f, 0.3739510f, 0.0000000f, 0.7960500f, 0.3623110f, 0.0000000f,
    0.7756100f, 0.3508630f, 0.0000000f, 0.7549300f, 0.3395540f, 0.0000000f,
    0.7339900f, 0.3283090f, 0.0000000f, 0.7127800f, 0.3171180f, 0.0000000f,
    0.6912900f, 0.3059360f, 0.0000000f, 0.6695200f, 0.2947370f, 0.0000000f,
    0.6474670f, 0.2834930f, 0.0000000f, 0.6251100f, 0.2722220f, 0.0000000f,
    0.6025200f, 0.2609900f, 0.0000000f, 0.5798900f, 0.2498770f, 0.0000000f,
    0.5573700f, 0.2389460f, 0.0000000f, 0.5351100f, 0.2282540f, 0.0000000f,
    0.5132400f, 0.2178530f, 0.0000000f, 0.4918600f, 0.2077800f, 0.0000000f,
    0.4710800f, 0.1980720f, 0.0000000f, 0.4509600f, 0.1887480f, 0.0000000f,
    /* 640 — 679 nm */
    0.4315670f, 0.1798280f, 0.0000000f, 0.4128700f, 0.1712850f, 0.0000000f,
    0.3947500f, 0.1630590f, 0.0000000f, 0.3772100f, 0.1551510f, 0.0000000f,
    0.3601900f, 0.1475350f, 0.0000000f, 0.3436900f, 0.1402110f, 0.0000000f,
    0.3276900f, 0.1331700f, 0.0000000f, 0.3121700f, 0.1264000f, 0.0000000f,
    0.2971100f, 0.1198920f, 0.0000000f, 0.2825000f, 0.1136400f, 0.0000000f,
    0.2683290f, 0.1076330f, 0.0000000f, 0.2545900f, 0.1018700f, 0.0000000f,
    0.2413000f, 0.0963470f, 0.0000000f, 0.2284800f, 0.0910630f, 0.0000000f,
    0.2161400f, 0.0860100f, 0.0000000f, 0.2043000f, 0.0811870f, 0.0000000f,
    0.1929500f, 0.0765830f, 0.0000000f, 0.1821100f, 0.0721980f, 0.0000000f,
    0.1717700f, 0.0680240f, 0.0000000f, 0.1619200f, 0.0640520f, 0.0000000f,
    0.1525680f, 0.0602810f, 0.0000000f, 0.1436700f, 0.0566970f, 0.0000000f,
    0.1352000f, 0.0532920f, 0.0000000f, 0.1271300f, 0.0500590f, 0.0000000f,
    0.1194800f, 0.0469980f, 0.0000000f, 0.1122100f, 0.0440960f, 0.0000000f,
    0.1053100f, 0.0413450f, 0.0000000f, 0.0987860f, 0.0387507f, 0.0000000f,
    0.0926100f, 0.0362978f, 0.0000000f, 0.0867730f, 0.0339832f, 0.0000000f,
    0.0812606f, 0.0318004f, 0.0000000f, 0.0760480f, 0.0297395f, 0.0000000f,
    0.0711140f, 0.0277918f, 0.0000000f, 0.0664540f, 0.0259551f, 0.0000000f,
    0.0620620f, 0.0242263f, 0.0000000f, 0.0579300f, 0.0226017f, 0.0000000f,
    0.0540500f, 0.0210779f, 0.0000000f, 0.0504120f, 0.0196505f, 0.0000000f,
    0.0470060f, 0.0183153f, 0.0000000f, 0.0438230f, 0.0170686f, 0.0000000f,
    /* 680 — 719 nm */
    0.0408508f, 0.0159051f, 0.0000000f, 0.0380720f, 0.0148183f, 0.0000000f,
    0.0354680f, 0.0138008f, 0.0000000f, 0.0330310f, 0.0128495f, 0.0000000f,
    0.0307530f, 0.0119607f, 0.0000000f, 0.0286230f, 0.0111303f, 0.0000000f,
    0.0266350f, 0.0103555f, 0.0000000f, 0.0247810f, 0.0096332f, 0.0000000f,
    0.0230520f, 0.0089599f, 0.0000000f, 0.0214410f, 0.0083324f, 0.0000000f,
    0.0199413f, 0.0077488f, 0.0000000f, 0.0185440f, 0.0072046f, 0.0000000f,
    0.0172410f, 0.0066975f, 0.0000000f, 0.0160270f, 0.0062251f, 0.0000000f,
    0.0148960f, 0.0057850f, 0.0000000f, 0.0138420f, 0.0053751f, 0.0000000f,
    0.0128620f, 0.0049941f, 0.0000000f, 0.0119490f, 0.0046392f, 0.0000000f,
    0.0111000f, 0.0043093f, 0.0000000f, 0.0103110f, 0.0040028f, 0.0000000f,
    0.0095769f, 0.0037177f, 0.0000000f, 0.0088940f, 0.0034526f, 0.0000000f,
    0.0082581f, 0.0032058f, 0.0000000f, 0.0076664f, 0.0029762f, 0.0000000f,
    0.0071163f, 0.0027628f, 0.0000000f, 0.0066052f, 0.0025646f, 0.0000000f,
    0.0061306f, 0.0023805f, 0.0000000f, 0.0056903f, 0.0022097f, 0.0000000f,
    0.0052819f, 0.0020513f, 0.0000000f, 0.0049033f, 0.0019045f, 0.0000000f,
    0.0045526f, 0.0017685f, 0.0000000f, 0.0042275f, 0.0016424f, 0.0000000f,
    0.0039258f, 0.0015254f, 0.0000000f, 0.0036457f, 0.0014167f, 0.0000000f,
    0.0033859f, 0.0013160f, 0.0000000f, 0.0031447f, 0.0012224f, 0.0000000f,
    0.0029208f, 0.0011356f, 0.0000000f, 0.0027130f, 0.0010549f, 0.0000000f,
    0.0025202f, 0.0009801f, 0.0000000f, 0.0023411f, 0.0009107f, 0.0000000f,
    /* 720 — 759 nm */
    0.0021750f, 0.0008462f, 0.0000000f, 0.0020206f, 0.0007863f, 0.0000000f,
    0.0018773f, 0.0007307f, 0.0000000f, 0.0017441f, 0.0006790f, 0.0000000f,
    0.0016205f, 0.0006310f, 0.0000000f, 0.0015057f, 0.0005864f, 0.0000000f,
    0.0013992f, 0.0005451f, 0.0000000f, 0.0013004f, 0.0005067f, 0.0000000f,
    0.0012087f, 0.0004711f, 0.0000000f, 0.0011236f, 0.0004381f, 0.0000000f,
    0.0010448f, 0.0004074f, 0.0000000f, 0.0009716f, 0.0003790f, 0.0000000f,
    0.0009036f, 0.0003525f, 0.0000000f, 0.0008405f, 0.0003280f, 0.0000000f,
    0.0007819f, 0.0003052f, 0.0000000f, 0.0007275f, 0.0002840f, 0.0000000f,
    0.0006769f, 0.0002644f, 0.0000000f, 0.0006300f, 0.0002461f, 0.0000000f,
    0.0005864f, 0.0002291f, 0.0000000f, 0.0005459f, 0.0002134f, 0.0000000f,
    0.0005083f, 0.0001987f, 0.0000000f, 0.0004733f, 0.0001851f, 0.0000000f,
    0.0004408f, 0.0001725f, 0.0000000f, 0.0004106f, 0.0001607f, 0.0000000f,
    0.0003825f, 0.0001497f, 0.0000000f, 0.0003564f, 0.0001396f, 0.0000000f,
    0.0003321f, 0.0001301f, 0.0000000f, 0.0003096f, 0.0001213f, 0.0000000f,
    0.0002886f, 0.0001131f, 0.0000000f, 0.0002691f, 0.0001055f, 0.0000000f,
    0.0002510f, 9.843e-05f, 0.0000000f, 0.0002341f, 9.185e-05f, 0.0000000f,
    0.0002185f, 8.574e-05f, 0.0000000f, 2.0391e-4f, 8.005e-05f, 0.0000000f,
    1.9040e-4f, 7.475e-05f, 0.0000000f, 1.7770e-4f, 6.982e-05f, 0.0000000f,
    0.0001660f, 6.522e-05f, 0.0000000f, 0.0001550f, 6.094e-05f, 0.0000000f,
    0.0001448f, 5.694e-05f, 0.0000000f, 0.0001353f, 5.322e-05f, 0.0000000f,
    /* 760 — 799 nm */
    0.0001264f, 4.974e-05f, 0.0000000f, 0.0001181f, 4.649e-05f, 0.0000000f,
    0.0001104f, 4.346e-05f, 0.0000000f, 0.0001032f, 4.064e-05f, 0.0000000f,
    9.643e-05f, 3.800e-05f, 0.0000000f, 9.015e-05f, 3.554e-05f, 0.0000000f,
    8.429e-05f, 3.324e-05f, 0.0000000f, 7.883e-05f, 3.110e-05f, 0.0000000f,
    7.373e-05f, 2.910e-05f, 0.0000000f, 6.897e-05f, 2.723e-05f, 0.0000000f,
    6.453e-05f, 2.549e-05f, 0.0000000f, 6.038e-05f, 2.386e-05f, 0.0000000f,
    0.0000565f, 2.233e-05f, 0.0000000f, 5.288e-05f, 2.091e-05f, 0.0000000f,
    4.950e-05f, 1.958e-05f, 0.0000000f, 4.634e-05f, 1.834e-05f, 0.0000000f,
    4.339e-05f, 1.718e-05f, 0.0000000f, 4.063e-05f, 1.609e-05f, 0.0000000f,
    3.806e-05f, 1.508e-05f, 0.0000000f, 3.566e-05f, 1.413e-05f, 0.0000000f,
    3.341e-05f, 1.325e-05f, 0.0000000f, 3.132e-05f, 1.242e-05f, 0.0000000f,
    2.936e-05f, 1.165e-05f, 0.0000000f, 2.752e-05f, 1.093e-05f, 0.0000000f,
    2.581e-05f, 1.025e-05f, 0.0000000f, 2.421e-05f, 9.620e-06f, 0.0000000f,
    2.271e-05f, 9.028e-06f, 0.0000000f, 2.131e-05f, 8.474e-06f, 0.0000000f,
    1.999e-05f, 7.955e-06f, 0.0000000f, 1.876e-05f, 7.469e-06f, 0.0000000f,
    1.761e-05f, 7.013e-06f, 0.0000000f, 1.653e-05f, 6.586e-06f, 0.0000000f,
    1.552e-05f, 6.186e-06f, 0.0000000f, 1.457e-05f, 5.811e-06f, 0.0000000f,
    1.369e-05f, 5.459e-06f, 0.0000000f, 1.286e-05f, 5.130e-06f, 0.0000000f,
    1.208e-05f, 4.821e-06f, 0.0000000f, 1.135e-05f, 4.531e-06f, 0.0000000f,
    1.066e-05f, 4.259e-06f, 0.0000000f, 1.002e-05f, 4.004e-06f, 0.0000000f,
    /* 800 — 830 nm */
    9.414e-06f, 3.765e-06f, 0.0000000f, 8.848e-06f, 3.540e-06f, 0.0000000f,
    8.317e-06f, 3.329e-06f, 0.0000000f, 7.819e-06f, 3.131e-06f, 0.0000000f,
    7.352e-06f, 2.945e-06f, 0.0000000f, 6.913e-06f, 2.771e-06f, 0.0000000f,
    6.502e-06f, 2.607e-06f, 0.0000000f, 6.115e-06f, 2.453e-06f, 0.0000000f,
    5.753e-06f, 2.309e-06f, 0.0000000f, 5.413e-06f, 2.173e-06f, 0.0000000f,
    5.093e-06f, 2.046e-06f, 0.0000000f, 4.794e-06f, 1.927e-06f, 0.0000000f,
    4.513e-06f, 1.814e-06f, 0.0000000f, 4.248e-06f, 1.709e-06f, 0.0000000f,
    4.000e-06f, 1.610e-06f, 0.0000000f, 3.767e-06f, 1.517e-06f, 0.0000000f,
    3.548e-06f, 1.429e-06f, 0.0000000f, 3.342e-06f, 1.347e-06f, 0.0000000f,
    3.149e-06f, 1.269e-06f, 0.0000000f, 2.967e-06f, 1.197e-06f, 0.0000000f,
    2.795e-06f, 1.128e-06f, 0.0000000f, 2.635e-06f, 1.064e-06f, 0.0000000f,
    2.483e-06f, 1.003e-06f, 0.0000000f, 2.341e-06f, 9.462e-07f, 0.0000000f,
    2.208e-06f, 8.926e-07f, 0.0000000f, 2.082e-06f, 8.422e-07f, 0.0000000f,
    1.964e-06f, 7.946e-07f, 0.0000000f, 1.852e-06f, 7.498e-07f, 0.0000000f,
    1.747e-06f, 7.074e-07f, 0.0000000f, 1.647e-06f, 6.675e-07f, 0.0000000f,
    1.553e-06f, 6.297e-07f, 0.0000000f,
};

vec3 Color::WavelengthToCIExyY(float nm)
{
    nm -= 360.f;

    int i = (int)nm;
    if (i < 0 || i > 830 - 360)
        return vec3(0.0f);

    float t = nm - i, s = 1.0 - t;
    float x = s * cie_1931_xyz[i * 3 + 0] + t * cie_1931_xyz[i * 3 + 3];
    float y = s * cie_1931_xyz[i * 3 + 1] + t * cie_1931_xyz[i * 3 + 4];
    float z = s * cie_1931_xyz[i * 3 + 2] + t * cie_1931_xyz[i * 3 + 5];
    float normalize = 1.f / (x + y + z);

    return vec3(x * normalize, y * normalize, 100.0f);
}

} /* namespace lol */

