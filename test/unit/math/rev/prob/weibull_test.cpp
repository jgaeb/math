#include <stan/math/rev.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <limits>

namespace weibull_test_internal {
struct TestValue {
  double y;
  double alpha;
  double sigma;
  double value;
  double grad_y;
  double grad_alpha;
  double grad_sigma;

  TestValue(double _y, double _alpha, double _sigma, double _value,
            double _grad_y, double _grad_alpha, double _grad_sigma)
      : y(_y),
        alpha(_alpha),
        sigma(_sigma),
        value(_value),
        grad_y(_grad_y),
        grad_alpha(_grad_alpha),
        grad_sigma(_grad_sigma) {}
};

constexpr double infinity = std::numeric_limits<double>::infinity();

// Test data generated in Mathematica (Wolfram Cloud). The code can be re-ran
// at https://www.wolframcloud.com/obj/martin.modrak/Published/Weibull_test.nb
// but is also presented below for convenience:
//
// weibull[y_,alpha_,sigma_]:= Log[alpha] + (alpha - 1) * Log[y] -
//     alpha * Log[sigma] - (y / sigma)^alpha;
// weibulldy[y_,alpha_,sigma_]= D[weibull[y,alpha, sigma],y];
// weibulldalpha[y_,alpha_,sigma_]= D[weibull[y,alpha, sigma],alpha];
// weibulldsigma[y_,alpha_,sigma_]= D[weibull[y,alpha, sigma],sigma];
// myCForm[x_] := If[x < -1.7 *10^308,"-infinity",
//     If[x > 1.7 *10^308,"+infinity",CForm[x]]];
// out = OpenWrite["weibull_test.txt"]
// alphas= {3*10^-7,22*10^-3,4*10^-1,7,21, 6334, 92345};
// sigmas=  {2*10^-7,1*10^-3,82*10^-2,22, 32145};
// ys = {10^-7,2*10^-3,8,105,32879};
//  WriteString[out, "std::vector<TestValue> testValues = {"];
//    Block[{$MaxPrecision = 80, $MinPrecision = 40}, {
//      For[i = 1, i <= Length[alphas], i++, {
//        For[j = 1, j <= Length[sigmas], j++, {
//        For[k = 1, k <= Length[ys], k++, {
//          calpha = alphas[[i]];
//          csigma = sigmas[[j]];
//     cy=ys[[k]];
//          val = N[weibull[cy,calpha,csigma]];
//     dy = N[weibulldy[cy,calpha,csigma]];
//    dalpha = N[weibulldalpha[cy,calpha,csigma]];
//     dsigma = N[weibulldsigma[cy,calpha,csigma]];
//          WriteString[out,"  TestValue(",CForm[cy],",",CForm[calpha],",",
//            CForm[csigma],",", myCForm[val],","myCForm[dy],",",
//            myCForm[dalpha], ",",myCForm[dsigma],"),"]
//        }]
//      }]
//   }]
//    }];
//  WriteString[out,"};"];
//  Close[out];
//  FilePrint[%]

std::vector<TestValue> testValues = {
    TestValue(1.e-7, 3.e-7, 2.e-7, 0.09861228866808958, -9.999999999999376e6,
              3.333333333333189e6, -3.1191619886961064e-7),
    TestValue(0.002, 3.e-7, 2.e-7, -9.804875263871836, -500.00000000041445,
              3.333333333307884e6, 4.14465889364557e-6),
    TestValue(8, 3.e-7, 2.e-7, -18.098924903983832, -0.12500000000019693,
              3.3333333332414124e6, 7.87699618753912e-6),
    TestValue(105, 3.e-7, 2.e-7, -20.673443712465875, -0.009523809523826735,
              3.333333333212384e6, 9.035536183255033e-6),
    TestValue(32879, 3.e-7, 2.e-7, -26.420072797402533,
              -0.000030414550320944197, 3.333333333133245e6,
              0.000011621537077566302),
    TestValue(1.e-7, 3.e-7, 0.001, 0.09861228866429494, -9.999999999991711e6,
              3.3333333333078846e6, -8.289294883096973e-10),
    TestValue(0.002, 3.e-7, 0.001, -9.804875263868038, -500.00000000003115,
              3.3333333333331896e6, 6.238325269769876e-11),
    TestValue(8, 3.e-7, 0.001, -18.09892490397368, -0.1250000000001011,
              3.333333333309103e6, 8.088488042865515e-10),
    TestValue(105, 3.e-7, 0.001, -20.673443712453746, -0.009523809523819435,
              3.333333333293231e6, 1.0405562112913297e-9),
    TestValue(32879, 3.e-7, 0.001, -26.420072797386, -0.000030414550320920884,
              3.3333333332434595e6, 1.557755068600647e-9),
    TestValue(1.e-7, 3.e-7, 0.82, 0.09861228865670768, -9.999999999985673e6,
              3.333333333257303e6, -1.7472739057697919e-12),
    TestValue(0.002, 3.e-7, 0.82, -9.804875263869645, -499.99999999972925,
              3.333333333322475e6, -6.603093362853843e-13),
    TestValue(8, 3.e-7, 0.82, -18.09892490397028, -0.12500000000002562,
              3.333333333331777e6, 2.500126747961654e-13),
    TestValue(105, 3.e-7, 0.82, -20.673443712448794, -0.009523809523813683,
              3.33333333332627e6, 5.325821144702175e-13),
    TestValue(32879, 3.e-7, 0.82, -26.420072797377575, -0.000030414550320902516,
              3.3333333332996317e6, 1.1633111588602733e-12),
    TestValue(1.e-7, 3.e-7, 22, 0.09861228865150773, -9.99999999998271e6,
              3.3333333332226365e6, -7.858261127410071e-14),
    TestValue(0.002, 3.e-7, 22, -9.804875263871914, -499.99999999958123,
              3.3333333333073547e6, -3.80685173013101e-14),
    TestValue(8, 3.e-7, 22, -18.098924903970094, -0.12499999999998862,
              3.3333333333330266e6, -4.138366737501566e-15),
    TestValue(105, 3.e-7, 22, -20.673443712447842, -0.009523809523810864,
              3.333333333332601e6, 6.393756533766453e-15),
    TestValue(32879, 3.e-7, 22, -26.42007279737492, -0.000030414550320893514,
              3.3333333333173045e6, 2.990272498566011e-14),
    TestValue(1.e-7, 3.e-7, 32145, 0.09861228863652038, -9.999999999976154e6,
              3.333333333122721e6, -7.418386152613115e-17),
    TestValue(0.002, 3.e-7, 32145, -9.804875263880408, -499.9999999992533,
              3.333333333250739e6, -4.645612411825573e-17),
    TestValue(8, 3.e-7, 32145, -18.098924903973145, -0.12499999999990664,
              3.333333333312674e6, -2.323441994864612e-17),
    TestValue(105, 3.e-7, 32145, -20.673443712449206, -0.009523809523804619,
              3.333333333323504e6, -1.6026262990453628e-17),
    TestValue(32879, 3.e-7, 32145, -26.42007279737252, -0.000030414550320873565,
              3.3333333333333335e6, 6.321205350557413e-20),
    TestValue(1.e-7, 0.022, 2.e-7, 11.301267144466628, -9.996670617437022e6,
              45.44405567210666, -1664.6912814894313),
    TestValue(0.002, 0.022, 2.e-7, 1.375906561716798, -502.47077819191554,
              43.385753806254726, 24707.781919155357),
    TestValue(8, 0.022, 2.e-7, -6.980814051075068, -0.12629182972610212,
              37.231748598222694, 51673.189044084575),
    TestValue(105, 0.022, 2.e-7, -9.58434223176885, -0.009640180105722611,
              34.30261809632128, 61094.55550437106),
    TestValue(32879, 0.022, 2.e-7, -15.414160095234973,
              -0.000030926440363328784, 25.697501008566725, 84152.16352943605),
    TestValue(1.e-7, 0.022, 0.001, 11.28217296579243, -9.95964812169889e6,
              43.76520666453736, -4.035187830110964),
    TestValue(0.002, 0.022, 0.001, 1.3977784099012593, -500.16902710956373,
              45.44389448505161, 0.3380542191274536),
    TestValue(8, 0.022, 0.001, -6.917055134350234, -0.12560120251702814,
              43.489772600142096, 4.809620136225078),
    TestValue(105, 0.022, 0.001, -9.505948511985501, -0.009584494550104714,
              42.10589014627066, 6.37192776099506),
    TestValue(32879, 0.022, 0.001, -15.299949537854905,
              -0.000030724641228105484, 37.4333243636762, 10.195478938880264),
    TestValue(1.e-7, 0.022, 0.82, 11.246626150019775, -9.934995388162024e6,
              40.75068034037598, -0.007927391687557996),
    TestValue(0.002, 0.022, 0.82, 1.3895102668403005, -498.63632503289176,
              44.708719734830034, -0.0033260365051420333),
    TestValue(8, 0.022, 0.82, -6.897431296636306, -0.1251413240507292,
              45.337483275470454, 0.001378771226626399),
    TestValue(105, 0.022, 0.82, -9.476579574894211, -0.00954741436042192,
              44.90787547985134, 0.003022570541831294),
    TestValue(32879, 0.022, 0.82, -15.246730697190106, -0.00003059026616209646,
              42.671159817511196, 0.0070455623702069085),
    TestValue(1.e-7, 0.022, 22, 11.223441750497818, -9.924174808039177e6,
              38.83392461744655, -0.00034465996345828427),
    TestValue(0.002, 0.022, 22, 1.3782990249108038, -497.96359129323235,
              43.73180838129245, -0.00018512806425160448),
    TestValue(8, 0.022, 22, -6.896400187740558, -0.12493947415104494,
              45.43228072582434, -0.000022009399620024923),
    TestValue(105, 0.022, 22, -9.471271146063124, -0.009531139120078571,
              45.39987120434333, 0.00003498216401136289),
    TestValue(32879, 0.022, 22, -15.230954066439203, -0.00003053128624548441,
              44.17930844343752, 0.0001744618393310013),
    TestValue(1.e-7, 0.022, 32145, 11.160199463148082, -9.902819177698947e6,
              33.750392961814754, -3.0232018136896315e-7),
    TestValue(0.002, 0.022, 32145, 1.3386871090057593, -496.6358756903292,
              40.38003297313277, -2.093093364237538e-7),
    TestValue(8, 0.022, 32145, -6.911850449117736, -0.12454110070036752,
              44.069742450370015, -1.1420732297588676e-7),
    TestValue(105, 0.022, 32145, -9.478279636079712, -0.009499018105075216,
              44.77726029777039, -8.097990253856948e-8),
    TestValue(32879, 0.022, 32145, -15.217302384081627,
              -0.000030414882754879957, 45.45453423766261,
              3.400248156196148e-10),
    TestValue(1.e-7, 0.4, 2.e-7, 14.166687763604989, -9.031433133020796e6,
              2.3321601517423964, -484283.43348960206),
    TestValue(0.002, 0.4, 2.e-7, -30.82826354001121, -8262.143411069945,
              -354.9599141602322, 7.762143411069945e7),
    TestValue(8, 0.4, 2.e-7, -1094.5545195748405, -55.00302716530589,
              -19209.627811898747, 2.1951210866122355e9),
    TestValue(105, 0.4, 2.e-7, -3074.0868722027844, -11.72589784628181,
              -61751.151572792995, 6.151096369297951e9),
    TestValue(32879, 0.4, 2.e-7, -30645.691440845785, -0.37283621491944424,
              -791387.6592528416, 6.128740955168204e10),
    TestValue(1.e-7, 0.4, 0.001, 11.492549905978597, -6.100475457260383e6,
              -6.4789870818766655, -389.9524542739617),
    TestValue(0.002, 0.4, 0.001, 4.25606832799912, -563.9015821545788,
              2.27853399248117, 127.80316430915764),
    TestValue(8, 0.4, 0.001, -35.812137605810804, -1.8955642030260802,
              -315.74817952427776, 14164.513624208641),
    TestValue(105, 0.4, 0.001, -102.91633973187422, -0.39417438057713683,
              -1164.8953862649964, 40388.30996059936),
    TestValue(32879, 0.4, 0.001, -1020.0981908261722, -0.012375128787917411,
              -17560.35783997773, 405881.8594179366),
    TestValue(1.e-7, 0.4, 0.82, 8.832231203391604, -6.006863323195077e6,
              -13.39232929553177, -0.48696788741523456),
    TestValue(0.002, 0.4, 0.82, 2.8017209566563084, -318.0267092024773,
              -2.9738995815269327, -0.4438372946281041),
    TestValue(8, 0.4, 0.82, -4.5717738199119, -0.19935992692596888,
              -0.8876783677606261, 0.7254627017167699),
    TestValue(105, 0.4, 0.82, -10.594752598523874, -0.03224939440778948,
              -26.446894717332203, 2.909983430265727),
    TestValue(32879, 0.4, 0.82, -76.45847873177931, -0.0008623281086936376,
              -722.2752555672529, 33.35669010455869),
    TestValue(1.e-7, 0.4, 22, 7.517689388006238, -6.001841157405093e6,
              -16.70029634260008, -0.0181734492845223),
    TestValue(0.002, 0.4, 22, 1.5518778902520198, -304.8358511167626,
              -6.5806468487153555, -0.01774219535302158),
    TestValue(8, 0.4, 22, -4.0675893606461555, -0.10836083612103861,
              2.1633561330095, -0.006050605046895049),
    TestValue(105, 0.4, 22, -6.8136422004492685, -0.012832602960522948,
              1.142514724249069, 0.01579196867522316),
    TestValue(32879, 0.4, 22, -27.005289534723655, -0.0002446817501804409,
              -126.23740919170123, 0.3202223301901234),
    TestValue(1.e-7, 0.4, 32145, 4.603336825239816, -6.000099819322826e6,
              -23.995446641648385, -0.000012443304341817308),
    TestValue(0.002, 0.4, 32145, -1.340041642585256, -300.2621782268201,
              -14.070869176161771, -0.000012427302645710369),
    TestValue(8, 0.4, 32145, -6.351334043125669, -0.07680867538066484,
              -5.498382246131589, -0.000011993485672878557),
    TestValue(105, 0.4, 32145, -7.96117669620024, -0.006100209049909547,
              -2.6441774861817224, -0.000011183016013672347),
    TestValue(32879, 0.4, 32145, -12.316921068525563, -0.000030524916171072216,
              2.499795183876087, 1.1288594769585807e-7),
    TestValue(1.e-7, 7, 2.e-7, 13.204163036094021, 5.9453125e7,
              -0.544874825354678, -3.47265625e7),
    TestValue(0.002, 7, 2.e-7, -1.e28, -3.5e31, -9.210340371976184e28, 3.5e35),
    TestValue(8, 7, 2.e-7, -1.6384e53, -1.4336e53, -2.867919259578894e54,
              5.7344e60),
    TestValue(105, 7, 2.e-7, -1.0992972052001953e61, -7.328648034667969e59,
              -2.207268834990665e62, 3.8475402182006834e68),
    TestValue(32879, 7, 2.e-7, -3.2450413242775404e78, -6.908753085538728e74,
              -8.380493772498085e79, 1.1357644634971392e86),
    TestValue(1.e-7, 7, 0.001, -46.40837680381964, 6.e7, -9.067483229119041,
              -6999.999999999999),
    TestValue(0.002, 7, 0.001, -114.98745148860289, -445000.,
              -87.88683478825591, 889000.),
    TestValue(8, 7, 0.001, -2.097152e27, -1.835008e27, -1.8847517786844897e28,
              1.4680064e31),
    TestValue(105, 7, 0.001, -1.40710042265625e35, -9.380669484375e33,
              -1.626849494839379e36, 9.84970295859375e38),
    TestValue(32879, 7, 0.001, -4.1536528950752517e52, -8.843203949489573e48,
              -7.1892856130534215e53, 2.9075570265526766e56),
    TestValue(1.e-7, 7, 0.82, -93.37350718562774, 5.999999999999999e7,
              -15.776787569377339, -8.536585365853659),
    TestValue(0.002, 7, 0.82, -33.95258187041097, 3000., -5.873300016841211,
              -8.536585365853659),
    TestValue(8, 7, 0.82, -8.41263653911822e6, -7.361070056979914e6,
              -1.9163115109465867e7, 7.181531640956014e7),
    TestValue(105, 7, 0.82, -5.644534434565652e14, -3.763022956377304e13,
              -2.738960261076741e15, 4.818505005117278e15),
    TestValue(32879, 7, 0.82, -1.66622342072983e32, -3.5474205252923787e28,
              -1.7660369308096857e33, 1.4223858469644892e33),
    TestValue(1.e-7, 7, 22, -116.39996093020282, 6.000000000000001e7,
              -19.066280961459494, -0.3181818181818182),
    TestValue(0.002, 7, 22, -56.97903561498605, 3000., -9.162793408923365,
              -0.31818181818181823),
    TestValue(8, 7, 22, -7.215578532634795, 0.7492643365217044,
              -0.8678932569981005, -0.31791430418971073),
    TestValue(105, 7, 22, -56403.09614460424, -3760.6980917882393,
              -88164.56915058891, 17948.740892625687),
    TestValue(32879, 7, 22, -1.6652192995471434e22, -3.5452827326956426e18,
              -1.2171998704913364e23, 5.298425044013638e21),
    TestValue(1.e-7, 7, 32145, -167.4087491327251, 5.999999999999999e7,
              -26.353250704676967, -0.00021776326022709597),
    TestValue(0.002, 7, 32145, -107.98782381750836, 2999.9999999999995,
              -16.449763152140836, -0.00021776326022709597),
    TestValue(8, 7, 32145, -58.223525976896184, 0.75, -8.155713512038808,
              -0.000217763260227096),
    TestValue(105, 7, 32145, -42.77641312603006, 0.05714285714285715,
              -5.5811947035611205, -0.00021776326022709597),
    TestValue(32879, 7, 32145, -9.467852442954618, -0.00006686629110717594,
              0.13899160746124167, 0.00003728408104877392),
    TestValue(1.e-7, 21, 2.e-7, 4.60652682008574, 1.9999989986419678e8,
              -0.645527802422566, -1.0499994993209839e8),
    TestValue(0.002, 21, 2.e-7, -1.e84, -1.05e88, -9.210340371976184e84,
              1.05e92),
    TestValue(8, 21, 2.e-7, -4.398046511104e159, -1.1544872091648e160,
              -7.698512142162427e160, 4.6179488366592e167),
    TestValue(105, 21, 2.e-7, -1.3284504844673354e183, -2.6569009689346707e182,
              -2.6673836150242936e184, 1.394873008690702e191),
    TestValue(32879, 21, 2.e-7, -3.417123657865132e235, -2.1825358683405143e232,
              -8.824905655391078e236, 3.587979840758388e243),
    TestValue(1.e-7, 21, 0.001, -174.25452972281812, 2.e8, -9.162721324357136,
              -21000.000000000004),
    TestValue(0.002, 21, 0.001, -2.0971281847786722e6, -2.2020086e10,
              -1.4536342552394222e6, 4.4040171e10),
    TestValue(8, 21, 0.001, -9.223372036854776e81, -2.4211351596743787e82,
              -8.289225984540379e82, 1.936908127739503e86),
    TestValue(105, 21, 0.001, -2.785962590401641e105, -5.5719251808032823e104,
              -3.221050722364507e106, 5.850521439843446e109),
    TestValue(32879, 21, 0.001, -7.166227713339177e157, -4.5771094613620466e154,
              -1.240355395619563e159, 1.5049078198012272e162),
    TestValue(1.e-7, 21, 0.82, -315.1499208682424, 2.e8, -15.872025664615435,
              -25.609756097560975),
    TestValue(0.002, 21, 0.82, -117.08016981751979, 10000., -5.9685381120793055,
              -25.609756097560975),
    TestValue(8, 21, 0.82, -5.953862857159315e20, -1.5628890000043202e21,
              -1.356225943167794e21, 1.5247697561017758e22),
    TestValue(105, 21, 0.82, -1.7983920763629985e44, -3.5967841527259974e43,
              -8.726538013178606e44, 4.605638244344265e45),
    TestValue(32879, 21, 0.82, -4.625936895737046e96, -2.9546116004281745e93,
              -4.903049192459643e97, 1.184691156225341e98),
    TestValue(1.e-7, 21, 22, -384.22928210196767, 2.e8, -19.16151905669759,
              -0.9545454545454546),
    TestValue(0.002, 21, 22, -186.159531051245, 10000., -9.25803150416146,
              -0.9545454545454545),
    TestValue(8, 21, 22, -20.278538249798807, 2.4999999984399346,
              -0.9639818634582277, -0.9545454539781582),
    TestValue(105, 21, 22, -1.7951427241965612e14, -3.590285448393728e13,
              -2.8056606909561603e14, 1.7135453276424603e14),
    TestValue(32879, 21, 22, -4.617578708292781e66, -2.9492731796632623e63,
              -3.375240851007447e67, 4.407688767006745e66),
    TestValue(1.e-7, 21, 32145, -537.2556467095345, 2.0000000000000003e8,
              -26.44848879991506, -0.0006532897806812878),
    TestValue(0.002, 21, 32145, -339.18589565881194, 10000., -16.54500124737893,
              -0.0006532897806812879),
    TestValue(8, 21, 32145, -173.30490285677138, 2.5000000000000004,
              -8.250951607276903, -0.000653289780681288),
    TestValue(105, 21, 32145, -121.81452668721764, 0.1904761904761905,
              -5.676432798799216, -0.0006532897806812879),
    TestValue(32879, 21, 32145, -8.488547983233325, -0.0004178552536494177,
              0.03392362713319368, 0.0003962875372449589),
    TestValue(1.e-7, 6334, 2.e-7, -4365.522458788636, 6.333e10,
              -0.6929893024418525, -3.167e10),
    TestValue(0.002, 6334, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(8, 6334, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 6334, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 6334, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 6334, 0.001, -58313.42413321909, 6.332999999999999e10,
              -9.210182493858092, -6.334000000000001e6),
    TestValue(0.002, 6334, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(8, 6334, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 6334, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 6334, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 6334, 0.82, -100810.15782441515, 6.333e10,
              -15.91948683411639, -7724.39024390244),
    TestValue(0.002, 6334, 0.82, -38091.37115420385, 3.1665e6,
              -6.01599928158026, -7724.390243902439),
    TestValue(8, 6334, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 6334, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 6334, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 6334, 22, -121645.80896986352, 6.333e10,
              -19.208980226198545, -287.90909090909093),
    TestValue(0.002, 6334, 22, -58927.02229965221, 3.1665e6, -9.305492673662414,
              -287.90909090909093),
    TestValue(8, 6334, 22, -6400.805928886075, 791.6250000000001,
              -1.0114430335603875, -287.9090909090909),
    TestValue(105, 6334, 22, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 6334, 22, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 6334, 32145, -167801.475323403, 6.333e10,
              -26.495949969416014, -0.19704464146834655),
    TestValue(0.002, 6334, 32145, -105082.68865319167, 3.1665e6,
              -16.592462416879886, -0.19704464146834658),
    TestValue(8, 6334, 32145, -52556.47228242553, 791.625, -8.298412776777859,
              -0.19704464146834655),
    TestValue(105, 6334, 32145, -36252.04466833634, 60.31428571428572,
              -5.723893968300171, -0.19704464146834655),
    TestValue(32879, 6334, 32145, -1.2762842603426092e62,
              -2.4587075352079092e61, -2.881497414787437e60,
              2.5148497449090328e61),
    TestValue(1.e-7, 92345, 2.e-7, -63981.12500631495, 9.2344e11,
              -0.6931363516033153, -4.61725e11),
    TestValue(0.002, 92345, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(8, 92345, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 92345, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 92345, 2.e-7, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 92345, 0.001, -850501.3302676473, 9.234400000000001e11,
              -9.210329543019554, -9.234500000000001e7),
    TestValue(0.002, 92345, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(8, 92345, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 92345, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 92345, 0.001, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 92345, 0.82, -1.4700720395687998e6, 9.2344e11,
              -15.919633883277852, -112615.85365853658),
    TestValue(0.002, 92345, 0.82, -555544.3850174036, 4.6172e7,
              -6.016146330741723, -112615.85365853658),
    TestValue(8, 92345, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(105, 92345, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 92345, 0.82, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 92345, 22, -1.7738403068606264e6, 9.2344e11,
              -19.209127275360007, -4197.500000000001),
    TestValue(0.002, 92345, 22, -859312.6523092303, 4.6172e7,
              -9.305639722823877, -4197.5),
    TestValue(8, 92345, 22, -93406.93234364866, 11543., -1.0115900827218502,
              -4197.500000000001),
    TestValue(105, 92345, 22, -infinity, -infinity, -infinity, +infinity),
    TestValue(32879, 92345, 22, -infinity, -infinity, -infinity, +infinity),
    TestValue(1.e-7, 92345, 32145, -2.4467555277980436e6, 9.2344e11,
              -26.49609701857748, -2.8727640379530257),
    TestValue(0.002, 92345, 32145, -1.5322278732466474e6, 4.6172e7,
              -16.59260946604135, -2.8727640379530253),
    TestValue(8, 92345, 32145, -766322.153281066, 11542.999999999998,
              -8.298559825939321, -2.8727640379530253),
    TestValue(105, 92345, 32145, -528580.7884310024, 879.4666666666667,
              -5.724041017461634, -2.8727640379530253),
    TestValue(32879, 92345, 32145, -infinity, -infinity, -infinity, +infinity),
};

}  // namespace weibull_test_internal

TEST(ProbDistributionsWeibull, derivativesPrecomputed) {
  using stan::math::is_nan;
  using stan::math::value_of;
  using stan::math::var;
  using stan::math::weibull_lpdf;
  using weibull_test_internal::TestValue;
  using weibull_test_internal::testValues;

  for (TestValue t : testValues) {
    var y(t.y);
    var alpha(t.alpha);
    var sigma(t.sigma);
    var val = weibull_lpdf(y, alpha, sigma);

    std::vector<var> x;
    x.push_back(y);
    x.push_back(alpha);
    x.push_back(sigma);

    std::vector<double> gradients;
    val.grad(x, gradients);

    for (int i = 0; i < 3; ++i) {
      EXPECT_FALSE(is_nan(gradients[i]));
    }

    auto my_expect_near = [](double actual, double expected, const char* type,
                             const char* message) {
      if (std::isinf(expected)) {
        EXPECT_TRUE(std::isinf(actual))
            << type << " expected infinite, " << message;
        EXPECT_TRUE(std::signbit(actual) == std::signbit(expected))
            << type << " expected same sign, " << message;
      } else {
        double tolerance = std::max(fabs(expected * 1e-8), 1e-14);
        EXPECT_NEAR(actual, expected, tolerance) << type << message;
      }
    };

    std::ostringstream message_stream;
    message_stream << "y = " << t.y << ", alpha = " << t.alpha
                   << ", sigma = " << t.sigma;

    const char* message = message_stream.str().c_str();

    my_expect_near(value_of(val), t.value, "value", message);
    my_expect_near(gradients[0], t.grad_y, "grad_y", message);
    my_expect_near(gradients[1], t.grad_alpha, "grad_alpha", message);
    my_expect_near(gradients[2], t.grad_sigma, "grad_sigma", message);
  }
}