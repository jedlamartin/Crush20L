#include "Stage4.h"
#include <cmath>
//#include <mpir.h>

#ifdef RESAMPLE
Stage4::Stage4():Stage(true){}
#else
Stage4::Stage4() : Stage(false) {}
#endif

//void calculate_Gp(mpf_t& G, const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val);
//double calculate_A(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_B(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_C(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_D(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_E(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_F(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_G(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_H(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);
//double calculate_I(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G);

void Stage4::processBlock(juce::AudioBuffer<float>& buffer) {
    if (!crossfade.isSmoothing()) {
        ParameterSet4 updt;
        if (updatedParameters.read_and_pop(updt)) {
            if (crossfade.getTargetValue() == 1.0f) {
                updatedParams = updt;
                yUBuffer.clear();
                uUBuffer.clear();
                crossfade.setTargetValue(0.0f);
            }
            else {
                params = updt;
                yBuffer.clear();
                uBuffer.clear();
                crossfade.setTargetValue(1.0f);
            }
        }
    }

    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, CircularBuffer<double, 4>{0.0f});
    this->uBuffer.resize(channelNumber, CircularBuffer<double, 4>{0.0f});
    this->yUBuffer.resize(channelNumber, CircularBuffer<double, 4>{0.0f});
    this->uUBuffer.resize(channelNumber, CircularBuffer<double, 4>{0.0f});



    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    for (int channel = 0; channel < channelNumber; channel++) {
        auto channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float tmp = channelSamples[i];

            double yCurrent = params.A * this->yBuffer[channel][0] + params.B * this->yBuffer[channel][1] + params.C * this->yBuffer[channel][2] + params.D * this->yBuffer[channel][3] + params.E * channelSamples[i] + params.F * this->uBuffer[channel][0] + params.G * this->uBuffer[channel][1] + params.H * this->uBuffer[channel][2] + params.I * this->uBuffer[channel][3];
            double yUpdate = updatedParams.A * this->yUBuffer[channel][0] + updatedParams.B * this->yUBuffer[channel][1] + updatedParams.C * this->yUBuffer[channel][2] + updatedParams.D * this->yUBuffer[channel][3] + updatedParams.E * channelSamples[i] + updatedParams.F * this->uUBuffer[channel][0] + updatedParams.G * this->uUBuffer[channel][1] + updatedParams.H * this->uUBuffer[channel][2] + updatedParams.I * this->uUBuffer[channel][3];
            float mult = this->crossfade.getCurrentValue();
            channelSamples[i] = mult * yCurrent + (1.0f - mult) * yUpdate;
            this->crossfade.getNextValue();

            this->yBuffer[channel].push(yCurrent);
            this->uBuffer[channel].push(tmp);

            this->yUBuffer[channel].push(yUpdate);
            this->uUBuffer[channel].push(tmp);
        }
    }

}

void Stage4::configure(double sampleRate) {


    this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
    float T = 1 / this->sampleRate;
    float B = *this->bassParameter;
    float M = *this->midParameter;
    float TR2 = *this->trebleParameter;
    float TR1 = 250000.0 + 0.01 - TR2;

    //ParameterSet4 newValues;

    //mpf_t G;
    //mpf_init(G);
    //calculate_Gp(G, B, T, M, TR1, TR2);
    
    //newValues.A = calculate_A(B, T, M, TR1, TR2, G);
    //newValues.B = calculate_B(B, T, M, TR1, TR2, G);
    //newValues.C = calculate_C(B, T, M, TR1, TR2, G);
    //newValues.D = calculate_D(B, T, M, TR1, TR2, G);
    //
    //newValues.E = calculate_E(B, T, M, TR1, TR2, G);
    //newValues.F = calculate_F(B, T, M, TR1, TR2, G);
    //newValues.G = calculate_G(B, T, M, TR1, TR2, G);
    //newValues.H = calculate_H(B, T, M, TR1, TR2, G);
    //newValues.I = calculate_I(B, T, M, TR1, TR2, G);

    //mpf_clear(G);

    ParameterSet4 test;

    double Gtest = 1 / (+0.01130372134364115060754327587799047340032 * B + 176770.96143779244665226050442134242165157527552 * T + 407710206613.02374846017593738533909861978651871936512 * T * T + 23478958321140.09724709257790592017087107900272212443136 * T * T * T + 107202058536745.06086456415978275980716864993698512896000 * T * T * T * T + 4652974.25610011569672731944346465281885369008128 * B * T * T + 42010530.24657726931454402255062843824492402180096 * B * T * T * T + 6325555.63138143276307508604044905936477950574592 * M * T * T + 51025223.97642915191421683321950367572143879749632 * M * T * T * T + 10329.74961474354549005622405705894449796612096 * T * T * TR1 + 418039.07237296008461999752796079546326933569536 * T * T * TR2 + 47164.48471371490019791004350555983703227498496 * T * T * T * TR1 + 21487576.19206272780354115905636512824837462294528 * T * T * T * TR2 + 0.00001131502506498479101003352093765926912 * B * M + 26248.47838881785363919970910852840193794244608 * B * T + 0.00001146995253987116711295197747089506304 * B * TR1 + 0.00001148125626121480743280080908034506752 * B * TR2 + 176.94773239923024339149460994860557489143808 * M * T + 179.37053440011290886042131980972317307043840 * T * TR1 + 179.54730536155072104914711561853215705137152 * T * TR2 + 0.40475732197571864672837639450473057484800 * B * M * T + 0.00000000017770114876988807125090902736896 * B * M * TR1 + 0.00000000016639742742624693571011290857472 * B * M * TR2 + 0.00204291709559486522535675718158367326208 * B * T * TR1 + 0.02793595796000667400080707823118864875520 * B * T * TR2 + 0.00000000001146995253987116755547567685632 * B * TR1 * TR2 + 0.00277894349672059002745290402357315633152 * M * T * TR1 + 0.00260217253528279740216918412850079203328 * M * T * TR2 + 0.00017937053440011293006195788130128232448 * T * TR1 * TR2 + 3.86818963268595072133291275884641107574784 * B * M * T * T + 0.01846388093521996180115108102950195560448 * B * T * T * TR1 + 4.11442562079591658212110268253557902802944 * B * T * T * TR2 + 0.02243076175434536536429345425399983636480 * M * T * T * TR1 + 5.92170904575866859795197328144624095592448 * M * T * T * TR2 + 0.00943289694274297973566772905655110991872 * T * T * TR1 * TR2 + 0.00000000170009177962517313516538236502016 * B * M * T * TR1 + 0.00000037866710904851354563064186697416704 * B * M * T * TR2 + 0.00000000000000016623119623001691580792832 * B * M * TR1 * TR2 + 0.00000000180619679849539623502563816505344 * B * T * TR1 * TR2 + 0.00000000259957296232047693535935788482560 * M * T * TR1 * TR2);
    test.A = -(-0.04521488537456460243017310351196189360128 * B - 353541.92287558489330452100884268484330315055104 * T + 46957916642280.19449418515581184034174215800544424886272 * T * T * T + 428808234146980.24345825663913103922867459974794051584000 * T * T * T * T + 84021060.49315453862908804510125687648984804360192 * B * T * T * T + 102050447.95285830382843366643900735144287759499264 * M * T * T * T + 94328.96942742980039582008701111967406454996992 * T * T * T * TR1 + 42975152.38412545560708231811273025649674924589056 * T * T * T * TR2 - 0.00004526010025993916404013408375063707648 * B * M - 52496.95677763570727839941821705680387588489216 * B * T - 0.00004587981015948466845180790988358025216 * B * TR1 - 0.00004592502504485922973120323632138027008 * B * TR2 - 353.89546479846048678298921989721114978287616 * M * T - 358.74106880022581772084263961944634614087680 * T * TR1 - 359.09461072310144209829423123706431410274304 * T * TR2 - 0.80951464395143729345675278900946114969600 * B * M * T - 0.00000000071080459507955228500363610947584 * B * M * TR1 - 0.00000000066558970970498774284045163429888 * B * M * TR2 - 0.00408583419118973045071351436316734652416 * B * T * TR1 - 0.05587191592001334800161415646237729751040 * B * T * TR2 - 0.00000000004587981015948467022190270742528 * B * TR1 * TR2 - 0.00555788699344118005490580804714631266304 * M * T * TR1 - 0.00520434507056559480433836825700158406656 * M * T * TR2 - 0.00035874106880022586012391576260256464896 * T * TR1 * TR2 - 0.00000000340018355925034627033076473004032 * B * M * T * TR1 - 0.00000075733421809702709126128373394833408 * B * M * T * TR2 - 0.00000000000000066492478492006766323171328 * B * M * TR1 * TR2 - 0.00000000361239359699079247005127633010688 * B * T * TR1 * TR2 - 0.00000000519914592464095387071871576965120 * M * T * TR1 * TR2) * Gtest;
    test.B = -(+0.06782232806184690364525965526794284040192 * B - 815420413226.04749692035187477067819723957303743873024 * T * T + 643212351220470.36518738495869655884301189962191077376000 * T * T * T * T - 9305948.51220023139345463888692930563770738016256 * B * T * T - 12651111.26276286552615017208089811872955901149184 * M * T * T - 20659.49922948709098011244811411788899593224192 * T * T * TR1 - 836078.14474592016923999505592159092653867139072 * T * T * TR2 + 0.00006789015038990874606020112562595561472 * B * M + 0.00006881971523922700267771186482537037824 * B * TR1 + 0.00006888753756728884459680485448207040512 * B * TR2 + 0.00000000106620689261932842750545416421376 * B * M * TR1 + 0.00000000099838456455748161426067745144832 * B * M * TR2 + 0.00000000006881971523922700533285406113792 * B * TR1 * TR2 - 7.73637926537190144266582551769282215149568 * B * M * T * T - 0.03692776187043992360230216205900391120896 * B * T * T * TR1 - 8.22885124159183316424220536507115805605888 * B * T * T * TR2 - 0.04486152350869073072858690850799967272960 * M * T * T * TR1 - 11.84341809151733719590394656289248191184896 * M * T * T * TR2 - 0.01886579388548595947133545811310221983744 * T * T * TR1 * TR2 + 0.00000000000000099738717738010149484756992 * B * M * TR1 * TR2) * Gtest;
    test.C = -(-0.04521488537456460243017310351196189360128 * B + 353541.92287558489330452100884268484330315055104 * T - 46957916642280.19449418515581184034174215800544424886272 * T * T * T + 428808234146980.24345825663913103922867459974794051584000 * T * T * T * T - 0.00004526010025993916404013408375063707648 * B * M + 52496.95677763570727839941821705680387588489216 * B * T - 0.00004587981015948466845180790988358025216 * B * TR1 - 0.00004592502504485922973120323632138027008 * B * TR2 + 353.89546479846048678298921989721114978287616 * M * T + 358.74106880022581772084263961944634614087680 * T * TR1 + 359.09461072310144209829423123706431410274304 * T * TR2 - 84021060.49315453862908804510125687648984804360192 * B * T * T * T - 102050447.95285830382843366643900735144287759499264 * M * T * T * T - 94328.96942742980039582008701111967406454996992 * T * T * T * TR1 - 42975152.38412545560708231811273025649674924589056 * T * T * T * TR2 + 0.80951464395143729345675278900946114969600 * B * M * T - 0.00000000071080459507955228500363610947584 * B * M * TR1 - 0.00000000066558970970498774284045163429888 * B * M * TR2 + 0.00408583419118973045071351436316734652416 * B * T * TR1 + 0.05587191592001334800161415646237729751040 * B * T * TR2 - 0.00000000004587981015948467022190270742528 * B * TR1 * TR2 + 0.00555788699344118005490580804714631266304 * M * T * TR1 + 0.00520434507056559480433836825700158406656 * M * T * TR2 + 0.00035874106880022586012391576260256464896 * T * TR1 * TR2 + 0.00000000340018355925034627033076473004032 * B * M * T * TR1 + 0.00000075733421809702709126128373394833408 * B * M * T * TR2 - 0.00000000000000066492478492006766323171328 * B * M * TR1 * TR2 + 0.00000000361239359699079247005127633010688 * B * T * TR1 * TR2 + 0.00000000519914592464095387071871576965120 * M * T * TR1 * TR2) * Gtest;
    test.D = -(+0.01130372134364115060754327587799047340032 * B - 176770.96143779244665226050442134242165157527552 * T - 176.94773239923024339149460994860557489143808 * M * T - 179.37053440011290886042131980972317307043840 * T * TR1 - 179.54730536155072104914711561853215705137152 * T * TR2 + 4652974.25610011569672731944346465281885369008128 * B * T * T - 42010530.24657726931454402255062843824492402180096 * B * T * T * T + 6325555.63138143276307508604044905936477950574592 * M * T * T - 51025223.97642915191421683321950367572143879749632 * M * T * T * T + 10329.74961474354549005622405705894449796612096 * T * T * TR1 + 418039.07237296008461999752796079546326933569536 * T * T * TR2 - 47164.48471371490019791004350555983703227498496 * T * T * T * TR1 - 21487576.19206272780354115905636512824837462294528 * T * T * T * TR2 + 407710206613.02374846017593738533909861978651871936512 * T * T - 23478958321140.09724709257790592017087107900272212443136 * T * T * T + 107202058536745.06086456415978275980716864993698512896000 * T * T * T * T + 0.00001131502506498479101003352093765926912 * B * M - 26248.47838881785363919970910852840193794244608 * B * T + 0.00001146995253987116711295197747089506304 * B * TR1 + 0.00001148125626121480743280080908034506752 * B * TR2 - 0.40475732197571864672837639450473057484800 * B * M * T + 0.00000000017770114876988807125090902736896 * B * M * TR1 + 0.00000000016639742742624693571011290857472 * B * M * TR2 - 0.00204291709559486522535675718158367326208 * B * T * TR1 - 0.02793595796000667400080707823118864875520 * B * T * TR2 + 0.00000000001146995253987116755547567685632 * B * TR1 * TR2 - 0.00277894349672059002745290402357315633152 * M * T * TR1 - 0.00260217253528279740216918412850079203328 * M * T * TR2 - 0.00017937053440011293006195788130128232448 * T * TR1 * TR2 + 3.86818963268595072133291275884641107574784 * B * M * T * T + 0.01846388093521996180115108102950195560448 * B * T * T * TR1 + 4.11442562079591658212110268253557902802944 * B * T * T * TR2 + 0.02243076175434536536429345425399983636480 * M * T * T * TR1 + 5.92170904575866859795197328144624095592448 * M * T * T * TR2 + 0.00943289694274297973566772905655110991872 * T * T * TR1 * TR2 - 0.00000000170009177962517313516538236502016 * B * M * T * TR1 - 0.00000037866710904851354563064186697416704 * B * M * T * TR2 + 0.00000000000000016623119623001691580792832 * B * M * TR1 * TR2 - 0.00000000180619679849539623502563816505344 * B * T * TR1 * TR2 - 0.00000000259957296232047693535935788482560 * M * T * TR1 * TR2) * Gtest;

    test.E = (+4027971.58756583523155850680214954774953359048704 * B * T * T + 5916962.45381602729876936696061507665516944162816 * M * T * T + 8842.08490585196215872031818855878091469750272 * T * T * TR2 + 0.00001130372134364115056986896299372642304 * B * M + 176.77096143779245537939396284922675731628032 * B * T + 0.00001130372134364115056986896299372642304 * B * TR2 + 176.77096143779245537939396284922675731628032 * M * T + 176.77096143779245537939396284922675731628032 * T * TR2 + 0.37862932923118854689210246793471159435264 * B * M * T + 0.00000000016623119623001691852489561337845 * B * M * TR1 + 0.00000000016623119623001691852489561337845 * B * M * TR2 + 0.00176841698117039259279305097898597810176 * B * T * TR1 + 0.00176841698117039259279305097898597810176 * B * T * TR2 + 0.00259957296232047714672908623097433686016 * M * T * TR1 + 0.00259957296232047714672908623097433686016 * M * T * TR2) * Gtest;
    test.F = (-0.00004521488537456460227947585197490569216 * B * M - 353.54192287558491075878792569845351463256064 * B * T - 0.00004521488537456460227947585197490569216 * B * TR2 - 353.54192287558491075878792569845351463256064 * M * T - 353.54192287558491075878792569845351463256064 * T * TR2 - 0.75725865846237709378420493586942318870528 * B * M * T - 0.00000000066492478492006767409958245351380 * B * M * TR1 - 0.00000000066492478492006767409958245351380 * B * M * TR2 - 0.00353683396234078518558610195797195620352 * B * T * TR1 - 0.00353683396234078518558610195797195620352 * B * T * TR2 - 0.00519914592464095429345817246194867372032 * M * T * TR1 - 0.00519914592464095429345817246194867372032 * M * T * TR2) * Gtest;
    test.G = (-8055943.17513167046311701360429909549906718097408 * B * T * T - 11833924.90763205459753873392123015331033888325632 * M * T * T - 17684.16981170392431744063637711756182939500544 * T * T * TR2 + 0.00006782232806184690341921377796235853824 * B * M + 0.00006782232806184690341921377796235853824 * B * TR2 + 0.00000000099738717738010151114937368027070 * B * M * TR1 + 0.00000000099738717738010151114937368027070 * B * M * TR2) * Gtest;
    test.H = (-0.00004521488537456460227947585197490569216 * B * M + 353.54192287558491075878792569845351463256064 * B * T - 0.00004521488537456460227947585197490569216 * B * TR2 + 353.54192287558491075878792569845351463256064 * M * T + 353.54192287558491075878792569845351463256064 * T * TR2 + 0.75725865846237709378420493586942318870528 * B * M * T - 0.00000000066492478492006767409958245351380 * B * M * TR1 - 0.00000000066492478492006767409958245351380 * B * M * TR2 + 0.00353683396234078518558610195797195620352 * B * T * TR1 + 0.00353683396234078518558610195797195620352 * B * T * TR2 + 0.00519914592464095429345817246194867372032 * M * T * TR1 + 0.00519914592464095429345817246194867372032 * M * T * TR2) * Gtest;
    test.I = (+4027971.58756583523155850680214954774953359048704 * B * T * T - 176.77096143779245537939396284922675731628032 * T * TR2 - 176.77096143779245537939396284922675731628032 * M * T + 5916962.45381602729876936696061507665516944162816 * M * T * T + 8842.08490585196215872031818855878091469750272 * T * T * TR2 + 0.00001130372134364115056986896299372642304 * B * M - 176.77096143779245537939396284922675731628032 * B * T + 0.00001130372134364115056986896299372642304 * B * TR2 - 0.37862932923118854689210246793471159435264 * B * M * T + 0.00000000016623119623001691852489561337845 * B * M * TR1 + 0.00000000016623119623001691852489561337845 * B * M * TR2 - 0.00176841698117039259279305097898597810176 * B * T * TR1 - 0.00176841698117039259279305097898597810176 * B * T * TR2 - 0.00259957296232047714672908623097433686016 * M * T * TR1 - 0.00259957296232047714672908623097433686016 * M * T * TR2) * Gtest;

    this->updatedParameters.push(test);
    
}

void Stage4::initParameters(std::atomic<float>* bassParameter, std::atomic<float>* midParameter, std::atomic<float>* trebleParameter) {
    this->bassParameter = bassParameter;
    this->midParameter = midParameter;
    this->trebleParameter = trebleParameter;
    this->crossfade.reset(CROSSFADE_SAMPLES);
    //mpf_set_default_prec(128);
}

//ParameterSet4::ParameterSet4(){
//    mpf_set_default_prec(128);
//    mpf_init(A);
//    mpf_init(B);
//    mpf_init(C);
//    mpf_init(D);
//    mpf_init(E);
//    mpf_init(F);
//    mpf_init(H);
//    mpf_init(I);
//}
//
//ParameterSet4::~ParameterSet4(){
//    mpf_clear(A);
//    mpf_clear(B);
//    mpf_clear(C);
//    mpf_clear(D);
//    mpf_clear(E);
//    mpf_clear(F);
//    mpf_clear(H);
//    mpf_clear(I);
//}

//void calculate_Gp(mpf_t& G, const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val) {
//    mpf_t B, T, M, TR1, TR2;
//
//    // Initialize the variables for B, T, M, TR1, TR2
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10;
//    mpf_t temp11, temp12, temp13, temp14, temp15, temp16, temp17, temp18, temp19, temp20;
//    mpf_t temp21, temp22, temp23, temp24, temp25, temp26, temp27, temp28, temp29, temp30;
//    mpf_t temp31, temp32, temp33, temp34, temp35, temp36, temp37, temp38, temp39, temp40;;
//
//    mpf_init(temp1);
//    mpf_init(temp2);
//    mpf_init(temp3);
//    mpf_init(temp4);
//    mpf_init(temp5);
//    mpf_init(temp6);
//    mpf_init(temp7);
//    mpf_init(temp8);
//    mpf_init(temp9);
//    mpf_init(temp10);
//
//    mpf_init(temp11);
//    mpf_init(temp12);
//    mpf_init(temp13);
//    mpf_init(temp14);
//    mpf_init(temp15);
//    mpf_init(temp16);
//    mpf_init(temp17);
//    mpf_init(temp18);
//    mpf_init(temp19);
//    mpf_init(temp20);
//
//    mpf_init(temp21);
//    mpf_init(temp22);
//    mpf_init(temp23);
//    mpf_init(temp24);
//    mpf_init(temp25);
//    mpf_init(temp26);
//    mpf_init(temp27);
//    mpf_init(temp28);
//    mpf_init(temp29);
//    mpf_init(temp30);
//
//    mpf_init(temp31);
//    mpf_init(temp32);
//    mpf_init(temp33);
//    mpf_init(temp34);
//    mpf_init(temp35);
//    mpf_init(temp36);
//    mpf_init(temp37);
//    mpf_init(temp38);
//    mpf_init(temp39);
//    mpf_init(temp40);
//
//
//
//
//
//
//    mpf_set_str(temp1, "0.01130372134364115060754327587799047340032", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//
//    mpf_set_str(temp2, "176770.96143779244665226050442134242165157527552", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "407710206613.02374846017593738533909861978651871936512", 10);
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T^2)
//
//    mpf_set_str(temp4, "23478958321140.09724709257790592017087107900272212443136", 10);
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T (T^2)
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T (T^3)
//
//    mpf_set_str(temp5, "107202058536745.06086456415978275980716864993698512896000", 10);
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T (T^2)
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T (T^3)
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T (T^4)
//
//    mpf_set_str(temp6, "4652974.25610011569672731944346465281885369008128", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T (T^2)
//
//    mpf_set_str(temp7, "42010530.24657726931454402255062843824492402180096", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T (T^2)
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T (T^3)
//
//    mpf_set_str(temp8, "6325555.63138143276307508604044905936477950574592", 10);
//    mpf_mul(temp8, temp8, M);  // temp8 = temp8 * M
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T (T^2)
//
//    mpf_set_str(temp9, "51025223.97642915191421683321950367572143879749632", 10);
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T (T^2)
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T (T^3)
//
//    mpf_set_str(temp10, "10329.74961474354549005622405705894449796612096", 10);
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T (T^2)
//    mpf_mul(temp10, temp10, TR1);  // temp10 = temp10 * TR1
//
//    mpf_set_str(temp11, "418039.07237296008461999752796079546326933569536", 10);
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T (T^2)
//    mpf_mul(temp11, temp11, TR2);  // temp11 = temp11 * TR2
//
//    mpf_set_str(temp12, "47164.48471371490019791004350555983703227498496", 10);
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T (T^2)
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T (T^3)
//    mpf_mul(temp12, temp12, TR1);  // temp12 = temp12 * TR1
//
//    mpf_set_str(temp13, "21487576.19206272780354115905636512824837462294528", 10);
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T (T^2)
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T (T^3)
//    mpf_mul(temp13, temp13, TR2);  // temp13 = temp13 * TR2
//
//    mpf_set_str(temp14, "0.00001131502506498479101003352093765926912", 10);
//    mpf_mul(temp14, temp14, B);  // temp14 = temp14 * B
//    mpf_mul(temp14, temp14, M);  // temp14 = temp14 * M
//
//    mpf_set_str(temp15, "26248.47838881785363919970910852840193794244608", 10);
//    mpf_mul(temp15, temp15, B);  // temp15 = temp15 * B
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//
//    mpf_set_str(temp16, "0.00001146995253987116711295197747089506304", 10);
//    mpf_mul(temp16, temp16, B);  // temp16 = temp16 * B
//    mpf_mul(temp16, temp16, TR1);  // temp16 = temp16 * TR1
//
//    mpf_set_str(temp17, "0.00001148125626121480743280080908034506752", 10);
//    mpf_mul(temp17, temp17, B);  // temp17 = temp17 * B
//    mpf_mul(temp17, temp17, TR2);  // temp17 = temp17 * TR2
//
//    mpf_set_str(temp18, "176.94773239923024339149460994860557489143808", 10);
//    mpf_mul(temp18, temp18, M);  // temp18 = temp18 * M
//    mpf_mul(temp18, temp18, T);  // temp18 = temp18 * T
//
//    mpf_set_str(temp19, "179.37053440011290886042131980972317307043840", 10);
//    mpf_mul(temp19, temp19, T);  // temp19 = temp19 * T
//    mpf_mul(temp19, temp19, TR1);  // temp19 = temp19 * TR1
//
//    mpf_set_str(temp20, "179.54730536155072104914711561853215705137152", 10);
//    mpf_mul(temp20, temp20, T);  // temp20 = temp20 * T
//    mpf_mul(temp20, temp20, TR2);  // temp20 = temp20 * TR2
//
//    mpf_set_str(temp21, "0.40475732197571864672837639450473057484800", 10);
//    mpf_mul(temp21, temp21, B);  // temp21 = temp21 * B
//    mpf_mul(temp21, temp21, M);  // temp21 = temp21 * M
//    mpf_mul(temp21, temp21, T);  // temp21 = temp21 * T
//
//    mpf_set_str(temp22, "0.00000000017770114876988807125090902736896", 10);
//    mpf_mul(temp22, temp22, B);  // temp22 = temp22 * B
//    mpf_mul(temp22, temp22, M);  // temp22 = temp22 * M
//    mpf_mul(temp22, temp22, TR1);  // temp22 = temp22 * TR1
//
//    mpf_set_str(temp23, "0.00000000016639742742624693571011290857472", 10);
//    mpf_mul(temp23, temp23, B);  // temp23 = temp23 * B
//    mpf_mul(temp23, temp23, M);  // temp23 = temp23 * M
//    mpf_mul(temp23, temp23, TR2);  // temp23 = temp23 * TR2
//
//    mpf_set_str(temp24, "0.00204291709559486522535675718158367326208", 10);
//    mpf_mul(temp24, temp24, B);  // temp24 = temp24 * B
//    mpf_mul(temp24, temp24, T);  // temp24 = temp24 * T
//    mpf_mul(temp24, temp24, TR1);  // temp24 = temp24 * TR1
//
//    mpf_set_str(temp25, "0.02793595796000667400080707823118864875520", 10);
//    mpf_mul(temp25, temp25, B);  // temp25 = temp25 * B
//    mpf_mul(temp25, temp25, T);  // temp25 = temp25 * T
//    mpf_mul(temp25, temp25, TR2);  // temp25 = temp25 * TR2
//
//    mpf_set_str(temp26, "0.00000000001146995253987116755547567685632", 10);
//    mpf_mul(temp26, temp26, B);  // temp26 = temp26 * B
//    mpf_mul(temp26, temp26, TR1);  // temp26 = temp26 * TR1
//    mpf_mul(temp26, temp26, TR2);  // temp26 = temp26 * TR2
//
//    mpf_set_str(temp27, "0.00277894349672059002745290402357315633152", 10);
//    mpf_mul(temp27, temp27, M);  // temp27 = temp27 * M
//    mpf_mul(temp27, temp27, T);  // temp27 = temp27 * T
//    mpf_mul(temp27, temp27, TR1);  // temp27 = temp27 * TR1
//
//    mpf_set_str(temp28, "0.00260217253528279740216918412850079203328", 10);
//    mpf_mul(temp28, temp28, M);  // temp28 = temp28 * M
//    mpf_mul(temp28, temp28, T);  // temp28 = temp28 * T
//    mpf_mul(temp28, temp28, TR2);  // temp28 = temp28 * TR2
//
//    mpf_set_str(temp29, "0.00017937053440011293006195788130128232448", 10);
//    mpf_mul(temp29, temp29, T);  // temp29 = temp29 * T
//    mpf_mul(temp29, temp29, TR1);  // temp29 = temp29 * TR1
//    mpf_mul(temp29, temp29, TR2);  // temp29 = temp29 * TR2
//
//    mpf_set_str(temp30, "3.86818963268595072133291275884641107574784", 10);
//    mpf_mul(temp30, temp30, B);  // temp30 = temp30 * B
//    mpf_mul(temp30, temp30, M);  // temp30 = temp30 * M
//    mpf_mul(temp30, temp30, T);  // temp30 = temp30 * T
//    mpf_mul(temp30, temp30, T);  // temp30 = temp30 * T (T^2)
//
//    mpf_set_str(temp31, "0.01846388093521996180115108102950195560448", 10);
//    mpf_mul(temp31, temp31, B);  // temp31 = temp31 * B
//    mpf_mul(temp31, temp31, T);  // temp31 = temp31 * T
//    mpf_mul(temp31, temp31, T);  // temp31 = temp31 * T (T^2)
//    mpf_mul(temp31, temp31, TR1);  // temp31 = temp31 * TR1
//
//    mpf_set_str(temp32, "4.11442562079591658212110268253557902802944", 10);
//    mpf_mul(temp32, temp32, B);  // temp32 = temp32 * B
//    mpf_mul(temp32, temp32, T);  // temp32 = temp32 * T
//    mpf_mul(temp32, temp32, T);  // temp32 = temp32 * T (T^2)
//    mpf_mul(temp32, temp32, TR2);  // temp32 = temp32 * TR2
//
//    mpf_set_str(temp33, "0.02243076175434536536429345425399983636480", 10);
//    mpf_mul(temp33, temp33, M);  // temp33 = temp33 * M
//    mpf_mul(temp33, temp33, T);  // temp33 = temp33 * T
//    mpf_mul(temp33, temp33, T);  // temp33 = temp33 * T (T^2)
//    mpf_mul(temp33, temp33, TR1);  // temp33 = temp33 * TR1
//
//    mpf_set_str(temp34, "5.92170904575866859795197328144624095592448", 10);
//    mpf_mul(temp34, temp34, M);  // temp34 = temp34 * M
//    mpf_mul(temp34, temp34, T);  // temp34 = temp34 * T
//    mpf_mul(temp34, temp34, T);  // temp34 = temp34 * T (T^2)
//    mpf_mul(temp34, temp34, TR2);  // temp34 = temp34 * TR2
//
//    mpf_set_str(temp35, "0.00943289694274297973566772905655110991872", 10);
//    mpf_mul(temp35, temp35, T);  // temp35 = temp35 * T
//    mpf_mul(temp35, temp35, T);  // temp35 = temp35 * T (T^2)
//    mpf_mul(temp35, temp35, TR1);  // temp35 = temp35 * TR1
//    mpf_mul(temp35, temp35, TR2);  // temp35 = temp35 * TR2
//
//    mpf_set_str(temp36, "0.00000000170009177962517313516538236502016", 10);
//    mpf_mul(temp36, temp36, B);  // temp36 = temp36 * B
//    mpf_mul(temp36, temp36, M);  // temp36 = temp36 * M
//    mpf_mul(temp36, temp36, T);  // temp36 = temp36 * T
//    mpf_mul(temp36, temp36, TR1);  // temp36 = temp36 * TR1
//
//    mpf_set_str(temp37, "0.00000037866710904851354563064186697416704", 10);
//    mpf_mul(temp37, temp37, B);  // temp37 = temp37 * B
//    mpf_mul(temp37, temp37, M);  // temp37 = temp37 * M
//    mpf_mul(temp37, temp37, T);  // temp37 = temp37 * T
//    mpf_mul(temp37, temp37, TR2);  // temp37 = temp37 * TR2
//
//    mpf_set_str(temp38, "0.00000000000000016623119623001691580792832", 10);
//    mpf_mul(temp38, temp38, B);  // temp38 = temp38 * B
//    mpf_mul(temp38, temp38, M);  // temp38 = temp38 * M
//    mpf_mul(temp38, temp38, TR1);  // temp38 = temp38 * TR1
//    mpf_mul(temp38, temp38, TR2);  // temp38 = temp38 * TR2
//
//    mpf_set_str(temp39, "0.00000000180619679849539623502563816505344", 10);
//    mpf_mul(temp39, temp39, B);  // temp39 = temp39 * B
//    mpf_mul(temp39, temp39, T);  // temp39 = temp39 * T
//    mpf_mul(temp39, temp39, TR1);  // temp39 = temp39 * TR1
//    mpf_mul(temp39, temp39, TR2);  // temp39 = temp39 * TR2
//
//    mpf_set_str(temp40, "0.00000000259957296232047693535935788482560", 10);
//    mpf_mul(temp40, temp40, M);  // temp40 = temp40 * M
//    mpf_mul(temp40, temp40, T);  // temp40 = temp40 * T
//    mpf_mul(temp40, temp40, TR1);  // temp40 = temp40 * TR1
//    mpf_mul(temp40, temp40, TR2);  // temp40 = temp40 * TR2
//
//
//
//
//    // Add all terms to G
//    mpf_add(G, temp1, temp2);  // G = temp1 + temp2
//    mpf_add(G, G, temp3);    // G = G + temp3
//    mpf_add(G, G, temp4);    // G = G + temp4
//    mpf_add(G, G, temp5);    // G = G + temp5
//    mpf_add(G, G, temp6);    // G = G + temp6
//    mpf_add(G, G, temp7);    // G = G + temp7
//    mpf_add(G, G, temp8);    // G = G + temp8
//    mpf_add(G, G, temp9);    // G = G + temp9
//    mpf_add(G, G, temp10);   // G = G + temp10
//    mpf_add(G, G, temp11);   // G = G + temp11
//    mpf_add(G, G, temp12);   // G = G + temp12
//    mpf_add(G, G, temp13);   // G = G + temp13
//    mpf_add(G, G, temp14);   // G = G + temp14
//    mpf_add(G, G, temp15);   // G = G + temp15
//    mpf_add(G, G, temp16);   // G = G + temp16
//    mpf_add(G, G, temp17);   // G = G + temp17
//    mpf_add(G, G, temp18);   // G = G + temp18
//    mpf_add(G, G, temp19);   // G = G + temp19
//    mpf_add(G, G, temp20);   // G = G + temp20
//    mpf_add(G, G, temp21);   // G = G + temp21
//    mpf_add(G, G, temp22);   // G = G + temp22
//    mpf_add(G, G, temp23);   // G = G + temp23
//    mpf_add(G, G, temp24);   // G = G + temp24
//    mpf_add(G, G, temp25);   // G = G + temp25
//    mpf_add(G, G, temp26);   // G = G + temp26
//    mpf_add(G, G, temp27);   // G = G + temp27
//    mpf_add(G, G, temp28);   // G = G + temp28
//    mpf_add(G, G, temp29);   // G = G + temp29
//    mpf_add(G, G, temp30);   // G = G + temp30
//    mpf_add(G, G, temp31);   // G = G + temp31
//    mpf_add(G, G, temp32);   // G = G + temp32
//    mpf_add(G, G, temp33);   // G = G + temp33
//    mpf_add(G, G, temp34);   // G = G + temp34
//    mpf_add(G, G, temp35);   // G = G + temp35
//    mpf_add(G, G, temp36);   // G = G + temp36
//    mpf_add(G, G, temp37);   // G = G + temp37
//    mpf_add(G, G, temp38);   // G = G + temp38
//    mpf_add(G, G, temp39);   // G = G + temp39
//    mpf_add(G, G, temp40);   // G = G + temp40
//
//    //compute G
//    mpf_ui_div(G, 1, G);
//
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//
//    mpf_clear(temp1);   // Clear temp1
//    mpf_clear(temp2);   // Clear temp2
//    mpf_clear(temp3);   // Clear temp3
//    mpf_clear(temp4);   // Clear temp4
//    mpf_clear(temp5);   // Clear temp5
//    mpf_clear(temp6);   // Clear temp6
//    mpf_clear(temp7);   // Clear temp7
//    mpf_clear(temp8);   // Clear temp8
//    mpf_clear(temp9);   // Clear temp9
//    mpf_clear(temp10);  // Clear temp10
//    mpf_clear(temp11);  // Clear temp11
//    mpf_clear(temp12);  // Clear temp12
//    mpf_clear(temp13);  // Clear temp13
//    mpf_clear(temp14);  // Clear temp14
//    mpf_clear(temp15);  // Clear temp15
//    mpf_clear(temp16);  // Clear temp16
//    mpf_clear(temp17);  // Clear temp17
//    mpf_clear(temp18);  // Clear temp18
//    mpf_clear(temp19);  // Clear temp19
//    mpf_clear(temp20);  // Clear temp20
//    mpf_clear(temp21);  // Clear temp21
//    mpf_clear(temp22);  // Clear temp22
//    mpf_clear(temp23);  // Clear temp23
//    mpf_clear(temp24);  // Clear temp24
//    mpf_clear(temp25);  // Clear temp25
//    mpf_clear(temp26);  // Clear temp26
//    mpf_clear(temp27);  // Clear temp27
//    mpf_clear(temp28);  // Clear temp28
//    mpf_clear(temp29);  // Clear temp29
//    mpf_clear(temp30);  // Clear temp30
//    mpf_clear(temp31);  // Clear temp31
//    mpf_clear(temp32);  // Clear temp32
//    mpf_clear(temp33);  // Clear temp33
//    mpf_clear(temp34);  // Clear temp34
//    mpf_clear(temp35);  // Clear temp35
//    mpf_clear(temp36);  // Clear temp36
//    mpf_clear(temp37);  // Clear temp37
//    mpf_clear(temp38);  // Clear temp38
//    mpf_clear(temp39);  // Clear temp39
//    mpf_clear(temp40);  // Clear temp40
//}
//double calculate_A(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13,
//        temp14, temp15, temp16, temp17, temp18, temp19, temp20, temp21, temp22, temp23, temp24, temp25,
//        temp26, temp27, temp28, temp29;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7); mpf_init(temp8); mpf_init(temp9); mpf_init(temp10); mpf_init(temp11); mpf_init(temp12);
//    mpf_init(temp13); mpf_init(temp14); mpf_init(temp15); mpf_init(temp16); mpf_init(temp17); mpf_init(temp18);
//    mpf_init(temp19); mpf_init(temp20); mpf_init(temp21); mpf_init(temp22); mpf_init(temp23); mpf_init(temp24);
//    mpf_init(temp25); mpf_init(temp26); mpf_init(temp27); mpf_init(temp28); mpf_init(temp29);
//
//    // Calculate each term in the equation (each temp variable is one term in the sum)
//    mpf_set_str(temp1, "-0.04521488537456460243017310351196189360128", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//
//    mpf_set_str(temp2, "-353541.92287558489330452100884268484330315055104", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "46957916642280.19449418515581184034174215800544424886272", 10);
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//
//    mpf_set_str(temp4, "428808234146980.24345825663913103922867459974794051584000", 10);
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//
//    mpf_set_str(temp5, "84021060.49315453862908804510125687648984804360192", 10);
//    mpf_mul(temp5, temp5, B);  // temp5 = temp5 * B
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//
//    mpf_set_str(temp6, "102050447.95285830382843366643900735144287759499264", 10);
//    mpf_mul(temp6, temp6, M);  // temp6 = temp6 * M
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//
//
//    mpf_set_str(temp7, "94328.96942742980039582008701111967406454996992", 10);
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, TR1);  // temp7 = temp7 * TR1
//
//    mpf_set_str(temp8, "42975152.38412545560708231811273025649674924589056", 10);
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2
//
//    mpf_set_str(temp9, "-0.00004526010025993916404013408375063707648", 10);
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M
//
//    mpf_set_str(temp10, "-52496.95677763570727839941821705680387588489216", 10);
//    mpf_mul(temp10, temp10, B);  // temp10 = temp10 * B
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//
//    mpf_set_str(temp11, "-0.00004587981015948466845180790988358025216", 10);
//    mpf_mul(temp11, temp11, B);  // temp11 = temp11 * B
//    mpf_mul(temp11, temp11, TR1);  // temp11 = temp11 * TR1
//
//    mpf_set_str(temp12, "-0.00004592502504485922973120323632138027008", 10);
//    mpf_mul(temp12, temp12, B);  // temp12 = temp12 * B
//    mpf_mul(temp12, temp12, TR2);  // temp12 = temp12 * TR2
//
//    mpf_set_str(temp13, "-353.89546479846048678298921989721114978287616", 10);
//    mpf_mul(temp13, temp13, M);  // temp13 = temp13 * M
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//
//    mpf_set_str(temp14, "-358.74106880022581772084263961944634614087680", 10);
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, TR1);  // temp14 = temp14 * TR1
//
//    mpf_set_str(temp15, "-359.09461072310144209829423123706431410274304", 10);
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, TR2);  // temp15 = temp15 * TR2
//
//    mpf_set_str(temp16, "-0.80951464395143729345675278900946114969600", 10);
//    mpf_mul(temp16, temp16, B);  // temp16 = temp16 * B
//    mpf_mul(temp16, temp16, M);  // temp16 = temp16 * M
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//
//    mpf_set_str(temp17, "-0.00000000071080459507955228500363610947584", 10);
//    mpf_mul(temp17, temp17, B);  // temp17 = temp17 * B
//    mpf_mul(temp17, temp17, M);  // temp17 = temp17 * M
//    mpf_mul(temp17, temp17, TR1);  // temp17 = temp17 * TR1
//
//    mpf_set_str(temp18, "-0.00000000066558970970498774284045163429888", 10);
//    mpf_mul(temp18, temp18, B);  // temp18 = temp18 * B
//    mpf_mul(temp18, temp18, M);  // temp18 = temp18 * M
//    mpf_mul(temp18, temp18, TR2);  // temp18 = temp18 * TR2
//
//    mpf_set_str(temp19, "-0.00408583419118973045071351436316734652416", 10);
//    mpf_mul(temp19, temp19, B);  // temp19 = temp19 * B
//    mpf_mul(temp19, temp19, T);  // temp19 = temp19 * T
//    mpf_mul(temp19, temp19, TR1);  // temp19 = temp19 * TR1
//
//    mpf_set_str(temp20, "-0.05587191592001334800161415646237729751040", 10);
//    mpf_mul(temp20, temp20, B);  // temp20 = temp20 * B
//    mpf_mul(temp20, temp20, T);  // temp20 = temp20 * T
//    mpf_mul(temp20, temp20, TR2);  // temp20 = temp20 * TR2
//
//    mpf_set_str(temp21, "-0.00000000004587981015948467022190270742528", 10);
//    mpf_mul(temp21, temp21, B);  // temp21 = temp21 * B
//    mpf_mul(temp21, temp21, TR1);  // temp21 = temp21 * TR1
//    mpf_mul(temp21, temp21, TR2);  // temp21 = temp21 * TR2
//
//    mpf_set_str(temp22, "-0.00555788699344118005490580804714631266304", 10);
//    mpf_mul(temp22, temp22, M);  // temp22 = temp22 * M
//    mpf_mul(temp22, temp22, T);  // temp22 = temp22 * T
//    mpf_mul(temp22, temp22, TR1);  // temp22 = temp22 * TR1
//
//    mpf_set_str(temp23, "-0.00520434507056559480433836825700158406656", 10);
//    mpf_mul(temp23, temp23, M);  // temp23 = temp23 * M
//    mpf_mul(temp23, temp23, T);  // temp23 = temp23 * T
//    mpf_mul(temp23, temp23, TR2);  // temp23 = temp23 * TR2
//
//    mpf_set_str(temp24, "-0.00035874106880022586012391576260256464896", 10);
//    mpf_mul(temp24, temp24, T);  // temp24 = temp24 * T
//    mpf_mul(temp24, temp24, TR1);  // temp24 = temp24 * TR1
//    mpf_mul(temp24, temp24, TR2);  // temp24 = temp24 * TR2
//
//    mpf_set_str(temp25, "-0.00000000340018355925034627033076473004032", 10);
//    mpf_mul(temp25, temp25, B);  // temp25 = temp25 * B
//    mpf_mul(temp25, temp25, M);  // temp25 = temp25 * M
//    mpf_mul(temp25, temp25, T);  // temp25 = temp25 * T
//    mpf_mul(temp25, temp25, TR1);  // temp25 = temp25 * TR1
//
//    mpf_set_str(temp26, "-0.00000075733421809702709126128373394833408", 10);
//    mpf_mul(temp26, temp26, B);  // temp26 = temp26 * B
//    mpf_mul(temp26, temp26, M);  // temp26 = temp26 * M
//    mpf_mul(temp26, temp26, T);  // temp26 = temp26 * T
//    mpf_mul(temp26, temp26, TR2);  // temp26 = temp26 * TR2
//
//    mpf_set_str(temp27, "-0.00000000000000066492478492006766323171328", 10);
//    mpf_mul(temp27, temp27, B);  // temp27 = temp27 * B
//    mpf_mul(temp27, temp27, M);  // temp27 = temp27 * M
//    mpf_mul(temp27, temp27, TR1);  // temp27 = temp27 * TR1
//    mpf_mul(temp27, temp27, TR2);  // temp27 = temp27 * TR2
//
//    mpf_set_str(temp28, "-0.00000000361239359699079247005127633010688", 10);
//    mpf_mul(temp28, temp28, B);  // temp28 = temp28 * B
//    mpf_mul(temp28, temp28, T);  // temp28 = temp28 * T
//    mpf_mul(temp28, temp28, TR1);  // temp28 = temp28 * TR1
//    mpf_mul(temp28, temp28, TR2);  // temp28 = temp28 * TR2
//
//    mpf_set_str(temp29, "-0.00000000519914592464095387071871576965120", 10);
//    mpf_mul(temp29, temp29, M);  // temp29 = temp29 * M
//    mpf_mul(temp29, temp29, T);  // temp29 = temp29 * T
//    mpf_mul(temp29, temp29, TR1);  // temp29 = temp29 * TR1
//    mpf_mul(temp29, temp29, TR2);  // temp29 = temp29 * TR2
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//    mpf_add(res, res, temp13);  // res = res + temp13
//    mpf_add(res, res, temp14);  // res = res + temp14
//    mpf_add(res, res, temp15);  // res = res + temp15
//    mpf_add(res, res, temp16);  // res = res + temp16
//    mpf_add(res, res, temp17);  // res = res + temp17
//    mpf_add(res, res, temp18);  // res = res + temp18
//    mpf_add(res, res, temp19);  // res = res + temp19
//    mpf_add(res, res, temp20);  // res = res + temp20
//    mpf_add(res, res, temp21);  // res = res + temp21
//    mpf_add(res, res, temp22);  // res = res + temp22
//    mpf_add(res, res, temp23);  // res = res + temp23
//    mpf_add(res, res, temp24);  // res = res + temp24
//    mpf_add(res, res, temp25);  // res = res + temp25
//    mpf_add(res, res, temp26);  // res = res + temp26
//    mpf_add(res, res, temp27);  // res = res + temp27
//    mpf_add(res, res, temp28);  // res = res + temp28
//    mpf_add(res, res, temp29);  // res = res + temp29
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//    mpf_clear(temp13);
//    mpf_clear(temp14);
//    mpf_clear(temp15);
//    mpf_clear(temp16);
//    mpf_clear(temp17);
//    mpf_clear(temp18);
//    mpf_clear(temp19);
//    mpf_clear(temp20);
//    mpf_clear(temp21);
//    mpf_clear(temp22);
//    mpf_clear(temp23);
//    mpf_clear(temp24);
//    mpf_clear(temp25);
//    mpf_clear(temp26);
//    mpf_clear(temp27);
//    mpf_clear(temp28);
//    mpf_clear(temp29);
//
//    return -result;  // Return the final result
//}
//double calculate_B(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize variables
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set values for B, T, M, TR1, TR2, and G
//    mpf_set_d(B, B_val);  // Set B
//    mpf_set_d(T, T_val);  // Set T
//    mpf_set_d(M, M_val);  // Set M
//    mpf_set_d(TR1, TR1_val);  // Set TR1
//    mpf_set_d(TR2, TR2_val);  // Set TR2
//
//    // Temporary variables for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13;
//    mpf_t temp14, temp15, temp16, temp17, temp18, temp19, temp20;
//    mpf_init(temp1);  mpf_init(temp2);  mpf_init(temp3);  mpf_init(temp4);  mpf_init(temp5);
//    mpf_init(temp6);  mpf_init(temp7);  mpf_init(temp8);  mpf_init(temp9);  mpf_init(temp10);
//    mpf_init(temp11); mpf_init(temp12); mpf_init(temp13); mpf_init(temp14); mpf_init(temp15);
//    mpf_init(temp16); mpf_init(temp17); mpf_init(temp18); mpf_init(temp19); mpf_init(temp20);
//
//    // Perform the calculation (first term as an example)
//    mpf_set_str(temp1, "0.06782232806184690364525965526794284040192", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//
//    mpf_set_str(temp2, "-815420413226.04749692035187477067819723957303743873024", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T (T^2)
//
//    mpf_set_str(temp3, "643212351220470.36518738495869655884301189962191077376000", 10);
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T^2)
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T^3)
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T^4)
//
//    mpf_set_str(temp4, "-9305948.51220023139345463888692930563770738016256", 10);
//    mpf_mul(temp4, temp4, B);  // temp4 = temp4 * B
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T (T^2)
//
//    mpf_set_str(temp5, "-12651111.26276286552615017208089811872955901149184", 10);
//    mpf_mul(temp5, temp5, M);  // temp5 = temp5 * M
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T (T^2)
//
//    mpf_set_str(temp6, "-20659.49922948709098011244811411788899593224192", 10);
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T (T^2)
//    mpf_mul(temp6, temp6, TR1);  // temp6 = temp6 * TR1
//
//    mpf_set_str(temp7, "-836078.14474592016923999505592159092653867139072", 10);
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T (T^2)
//    mpf_mul(temp7, temp7, TR2);  // temp7 = temp7 * TR2
//
//    mpf_set_str(temp8, "0.00006789015038990874606020112562595561472", 10);
//    mpf_mul(temp8, temp8, B);  // temp8 = temp8 * B
//    mpf_mul(temp8, temp8, M);  // temp8 = temp8 * M
//
//    mpf_set_str(temp9, "0.00006881971523922700267771186482537037824", 10);
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, TR1);  // temp9 = temp9 * TR1
//
//    mpf_set_str(temp10, "0.00006888753756728884459680485448207040512", 10);
//    mpf_mul(temp10, temp10, B);  // temp10 = temp10 * B
//    mpf_mul(temp10, temp10, TR2);  // temp10 = temp10 * TR2
//
//    mpf_set_str(temp11, "0.00000000106620689261932842750545416421376", 10);
//    mpf_mul(temp11, temp11, B);  // temp11 = temp11 * B
//    mpf_mul(temp11, temp11, M);  // temp11 = temp11 * M
//    mpf_mul(temp11, temp11, TR1);  // temp11 = temp11 * TR1
//
//    mpf_set_str(temp12, "0.00000000099838456455748161426067745144832", 10);
//    mpf_mul(temp12, temp12, B);  // temp12 = temp12 * B
//    mpf_mul(temp12, temp12, M);  // temp12 = temp12 * M
//    mpf_mul(temp12, temp12, TR2);  // temp12 = temp12 * TR2
//
//    mpf_set_str(temp13, "0.00000000006881971523922700533285406113792", 10);
//    mpf_mul(temp13, temp13, B);  // temp13 = temp13 * B
//    mpf_mul(temp13, temp13, TR1);  // temp13 = temp13 * TR1
//    mpf_mul(temp13, temp13, TR2);  // temp13 = temp13 * TR2
//
//    mpf_set_str(temp14, "-7.73637926537190144266582551769282215149568", 10);
//    mpf_mul(temp14, temp14, B);  // temp14 = temp14 * B
//    mpf_mul(temp14, temp14, M);  // temp14 = temp14 * M
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T (T^2)
//
//    mpf_set_str(temp15, "-0.03692776187043992360230216205900391120896", 10);
//    mpf_mul(temp15, temp15, B);  // temp15 = temp15 * B
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T (T^2)
//    mpf_mul(temp15, temp15, TR1);  // temp15 = temp15 * TR1
//
//    mpf_set_str(temp16, "-8.22885124159183316424220536507115805605888", 10);
//    mpf_mul(temp16, temp16, B);  // temp16 = temp16 * B
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T (T^2)
//    mpf_mul(temp16, temp16, TR2);  // temp16 = temp16 * TR2
//
//    mpf_set_str(temp17, "-0.04486152350869073072858690850799967272960", 10);
//    mpf_mul(temp17, temp17, M);  // temp17 = temp17 * M
//    mpf_mul(temp17, temp17, T);  // temp17 = temp17 * T
//    mpf_mul(temp17, temp17, T);  // temp17 = temp17 * T (T^2)
//    mpf_mul(temp17, temp17, TR1);  // temp17 = temp17 * TR1
//
//    mpf_set_str(temp18, "-11.84341809151733719590394656289248191184896", 10);
//    mpf_mul(temp18, temp18, M);  // temp18 = temp18 * M
//    mpf_mul(temp18, temp18, T);  // temp18 = temp18 * T
//    mpf_mul(temp18, temp18, T);  // temp18 = temp18 * T (T^2)
//    mpf_mul(temp18, temp18, TR2);  // temp18 = temp18 * TR2
//
//    mpf_set_str(temp19, "-0.01886579388548595947133545811310221983744", 10);
//    mpf_mul(temp19, temp19, T);  // temp19 = temp19 * T
//    mpf_mul(temp19, temp19, T);  // temp19 = temp19 * T (T^2)
//    mpf_mul(temp19, temp19, TR1);  // temp19 = temp19 * TR1
//    mpf_mul(temp19, temp19, TR2);  // temp19 = temp19 * TR2
//
//    mpf_set_str(temp20, "0.00000000000000099738717738010149484756992", 10);
//    mpf_mul(temp20, temp20, B);  // temp20 = temp20 * B
//    mpf_mul(temp20, temp20, M);  // temp20 = temp20 * M
//    mpf_mul(temp20, temp20, TR1);  // temp20 = temp20 * TR1
//    mpf_mul(temp20, temp20, TR2);  // temp20 = temp20 * TR2
//
//
//    // Combine all terms
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//    mpf_add(res, res, temp13);  // res = res + temp13
//    mpf_add(res, res, temp14);  // res = res + temp14
//    mpf_add(res, res, temp15);  // res = res + temp15
//    mpf_add(res, res, temp16);  // res = res + temp16
//    mpf_add(res, res, temp17);  // res = res + temp17
//    mpf_add(res, res, temp18);  // res = res + temp18
//    mpf_add(res, res, temp19);  // res = res + temp19
//    mpf_add(res, res, temp20);  // res = res + temp20
//
//    // Multiply by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    double result = mpf_get_d(res);
//    // Clear variables
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//    mpf_clear(temp13);
//    mpf_clear(temp14);
//    mpf_clear(temp15);
//    mpf_clear(temp16);
//    mpf_clear(temp17);
//    mpf_clear(temp18);
//    mpf_clear(temp19);
//    mpf_clear(temp20);
//
//    return -result;
//}
//double calculate_C(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13;
//    mpf_t temp14, temp15, temp16, temp17, temp18, temp19, temp20, temp21, temp22, temp23, temp24, temp25, temp26;
//    mpf_t temp27, temp28, temp29;
//
//
//    // Initialize temp variables
//    mpf_init(temp1);  mpf_init(temp2);  mpf_init(temp3);  mpf_init(temp4);  mpf_init(temp5);
//    mpf_init(temp6);  mpf_init(temp7);  mpf_init(temp8);  mpf_init(temp9);  mpf_init(temp10);
//    mpf_init(temp11); mpf_init(temp12); mpf_init(temp13); mpf_init(temp14); mpf_init(temp15);
//    mpf_init(temp16); mpf_init(temp17); mpf_init(temp18); mpf_init(temp19); mpf_init(temp20);
//    mpf_init(temp21); mpf_init(temp22); mpf_init(temp23); mpf_init(temp24); mpf_init(temp25);
//    mpf_init(temp26); mpf_init(temp27); mpf_init(temp28); mpf_init(temp29); 
//
//    // Set constants for the calculation using string initializations
//    // temp1 = -0.04521488537456460243017310351196189360128 * B
//    mpf_set_str(temp1, "-0.04521488537456460243017310351196189360128", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//
//    // temp2 = 353541.92287558489330452100884268484330315055104 * T
//    mpf_set_str(temp2, "353541.92287558489330452100884268484330315055104", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    // temp3 = -46957916642280.19449418515581184034174215800544424886272 * T * T * T
//    mpf_set_str(temp3, "-46957916642280.19449418515581184034174215800544424886272", 10);
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T^3)
//
//    // temp4 = 428808234146980.24345825663913103922867459974794051584000 * T * T * T * T
//    mpf_set_str(temp4, "428808234146980.24345825663913103922867459974794051584000", 10);
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T (T^4)
//
//    // temp5 = -0.00004526010025993916404013408375063707648 * B * M
//    mpf_set_str(temp5, "-0.00004526010025993916404013408375063707648", 10);
//    mpf_mul(temp5, temp5, B);  // temp5 = temp5 * B
//    mpf_mul(temp5, temp5, M);  // temp5 = temp5 * M
//
//    // temp6 = 52496.95677763570727839941821705680387588489216 * B * T
//    mpf_set_str(temp6, "52496.95677763570727839941821705680387588489216", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//
//    // temp7 = -0.00004587981015948466845180790988358025216 * B * TR1
//    mpf_set_str(temp7, "-0.00004587981015948466845180790988358025216", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, TR1);  // temp7 = temp7 * TR1
//
//    // temp8 = -0.00004592502504485922973120323632138027008 * B * TR2
//    mpf_set_str(temp8, "-0.00004592502504485922973120323632138027008", 10);
//    mpf_mul(temp8, temp8, B);  // temp8 = temp8 * B
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2
//
//    // temp9 = 353.89546479846048678298921989721114978287616 * M * T
//    mpf_set_str(temp9, "353.89546479846048678298921989721114978287616", 10);
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//
//    // temp10 = 358.74106880022581772084263961944634614087680 * T * TR1
//    mpf_set_str(temp10, "358.74106880022581772084263961944634614087680", 10);
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, TR1);  // temp10 = temp10 * TR1
//
//    // temp11 = 359.09461072310144209829423123706431410274304 * T * TR2
//    mpf_set_str(temp11, "359.09461072310144209829423123706431410274304", 10);
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, TR2);  // temp11 = temp11 * TR2
//
//    // temp12 = -84021060.49315453862908804510125687648984804360192 * B * T * T * T
//    mpf_set_str(temp12, "-84021060.49315453862908804510125687648984804360192", 10);
//    mpf_mul(temp12, temp12, B);  // temp12 = temp12 * B
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T (T^3)
//
//    // temp13 = -102050447.95285830382843366643900735144287759499264 * M * T * T * T
//    mpf_set_str(temp13, "-102050447.95285830382843366643900735144287759499264", 10);
//    mpf_mul(temp13, temp13, M);  // temp13 = temp13 * M
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T (T^3)
//
//    // temp14 = 94328.96942742980039582008701111967406454996992 * T * T * T * TR1
//    mpf_set_str(temp14, "94328.96942742980039582008701111967406454996992", 10);
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T (T^3)
//    mpf_mul(temp14, temp14, TR1);  // temp14 = temp14 * TR1
//
//    // temp15 = -42975152.38412545560708231811273025649674924589056 * T * T * T * TR2
//    mpf_set_str(temp15, "-42975152.38412545560708231811273025649674924589056", 10);
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T (T^3)
//    mpf_mul(temp15, temp15, TR2);  // temp15 = temp15 * TR2
//
//    // temp16 = 0.80951464395143729345675278900946114969600 * B * M * T
//    mpf_set_str(temp16, "0.80951464395143729345675278900946114969600", 10);
//    mpf_mul(temp16, temp16, B);  // temp16 = temp16 * B
//    mpf_mul(temp16, temp16, M);  // temp16 = temp16 * M
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//
//    // temp17 = -0.00000000071080459507955228500363610947584 * B * M * TR1
//    mpf_set_str(temp17, "-0.00000000071080459507955228500363610947584", 10);
//    mpf_mul(temp17, temp17, B);  // temp17 = temp17 * B
//    mpf_mul(temp17, temp17, M);  // temp17 = temp17 * M
//    mpf_mul(temp17, temp17, TR1);  // temp17 = temp17 * TR1
//
//    // temp18 = -0.00000000066558970970498774284045163429888 * B * M * TR2
//    mpf_set_str(temp18, "-0.00000000066558970970498774284045163429888", 10);
//    mpf_mul(temp18, temp18, B);  // temp18 = temp18 * B
//    mpf_mul(temp18, temp18, M);  // temp18 = temp18 * M
//    mpf_mul(temp18, temp18, TR2);  // temp18 = temp18 * TR2
//
//    // temp19 = 0.00408583419118973045071351436316734652416 * B * T * TR1
//    mpf_set_str(temp19, "0.00408583419118973045071351436316734652416", 10);
//    mpf_mul(temp19, temp19, B);  // temp19 = temp19 * B
//    mpf_mul(temp19, temp19, T);  // temp19 = temp19 * T
//    mpf_mul(temp19, temp19, TR1);  // temp19 = temp19 * TR1
//
//    // temp20 = 0.05587191592001334800161415646237729751040 * B * T * TR2
//    mpf_set_str(temp20, "0.05587191592001334800161415646237729751040", 10);
//    mpf_mul(temp20, temp20, B);  // temp20 = temp20 * B
//    mpf_mul(temp20, temp20, T);  // temp20 = temp20 * T
//    mpf_mul(temp20, temp20, TR2);  // temp20 = temp20 * TR2
//
//    // temp21 = -0.00000000004587981015948467022190270742528 * B * TR1 * TR2
//    mpf_set_str(temp21, "-0.00000000004587981015948467022190270742528", 10);
//    mpf_mul(temp21, temp21, B);  // temp21 = temp21 * B
//    mpf_mul(temp21, temp21, TR1);  // temp21 = temp21 * TR1
//    mpf_mul(temp21, temp21, TR2);  // temp21 = temp21 * TR2
//
//    // temp22 = 0.00555788699344118005490580804714631266304 * M * T * TR1
//    mpf_set_str(temp22, "0.00555788699344118005490580804714631266304", 10);
//    mpf_mul(temp22, temp22, M);  // temp22 = temp22 * M
//    mpf_mul(temp22, temp22, T);  // temp22 = temp22 * T
//    mpf_mul(temp22, temp22, TR1);  // temp22 = temp22 * TR1
//
//    // temp23 = 0.00520434507056559480433836825700158406656 * M * T * TR2
//    mpf_set_str(temp23, "0.00520434507056559480433836825700158406656", 10);
//    mpf_mul(temp23, temp23, M);  // temp23 = temp23 * M
//    mpf_mul(temp23, temp23, T);  // temp23 = temp23 * T
//    mpf_mul(temp23, temp23, TR2);  // temp23 = temp23 * TR2
//
//    // temp24 = 0.00035874106880022586012391576260256464896 * T * TR1 * TR2
//    mpf_set_str(temp24, "0.00035874106880022586012391576260256464896", 10);
//    mpf_mul(temp24, temp24, T);  // temp24 = temp24 * T
//    mpf_mul(temp24, temp24, TR1);  // temp24 = temp24 * TR1
//    mpf_mul(temp24, temp24, TR2);  // temp24 = temp24 * TR2
//
//    // temp25 = 0.00000000340018355925034627033076473004032 * B * M * T * TR1
//    mpf_set_str(temp25, "0.00000000340018355925034627033076473004032", 10);
//    mpf_mul(temp25, temp25, B);  // temp25 = temp25 * B
//    mpf_mul(temp25, temp25, M);  // temp25 = temp25 * M
//    mpf_mul(temp25, temp25, T);  // temp25 = temp25 * T
//    mpf_mul(temp25, temp25, TR1);  // temp25 = temp25 * TR1
//
//    // temp26 = 0.00000075733421809702709126128373394833408 * B * M * T * TR2
//    mpf_set_str(temp26, "0.00000075733421809702709126128373394833408", 10);
//    mpf_mul(temp26, temp26, B);  // temp26 = temp26 * B
//    mpf_mul(temp26, temp26, M);  // temp26 = temp26 * M
//    mpf_mul(temp26, temp26, T);  // temp26 = temp26 * T
//    mpf_mul(temp26, temp26, TR2);  // temp26 = temp26 * TR2
//
//    // temp27 = -0.00000000000000066492478492006766323171328 * B * M * TR1 * TR2
//    mpf_set_str(temp27, "-0.00000000000000066492478492006766323171328", 10);
//    mpf_mul(temp27, temp27, B);  // temp27 = temp27 * B
//    mpf_mul(temp27, temp27, M);  // temp27 = temp27 * M
//    mpf_mul(temp27, temp27, TR1);  // temp27 = temp27 * TR1
//    mpf_mul(temp27, temp27, TR2);  // temp27 = temp27 * TR2
//
//    // temp28 = 0.00000000361239359699079247005127633010688 * B * T * TR1 * TR2
//    mpf_set_str(temp28, "0.00000000361239359699079247005127633010688", 10);
//    mpf_mul(temp28, temp28, B);  // temp28 = temp28 * B
//    mpf_mul(temp28, temp28, T);  // temp28 = temp28 * T
//    mpf_mul(temp28, temp28, TR1);  // temp28 = temp28 * TR1
//    mpf_mul(temp28, temp28, TR2);  // temp28 = temp28 * TR2
//
//    // temp29 = 0.00000000519914592464095387071871576965120 * M * T * TR1 * TR2
//    mpf_set_str(temp29, "0.00000000519914592464095387071871576965120", 10);
//    mpf_mul(temp29, temp29, M);  // temp29 = temp29 * M
//    mpf_mul(temp29, temp29, T);  // temp29 = temp29 * T
//    mpf_mul(temp29, temp29, TR1);  // temp29 = temp29 * TR1
//    mpf_mul(temp29, temp29, TR2);  // temp29 = temp29 * TR2
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//    mpf_add(res, res, temp13);  // res = res + temp13
//    mpf_add(res, res, temp14);  // res = res + temp14
//    mpf_add(res, res, temp15);  // res = res + temp15
//    mpf_add(res, res, temp16);  // res = res + temp16
//    mpf_add(res, res, temp17);  // res = res + temp17
//    mpf_add(res, res, temp18);  // res = res + temp18
//    mpf_add(res, res, temp19);  // res = res + temp19
//    mpf_add(res, res, temp20);  // res = res + temp20
//    mpf_add(res, res, temp21);  // res = res + temp21
//    mpf_add(res, res, temp22);  // res = res + temp22
//    mpf_add(res, res, temp23);  // res = res + temp23
//    mpf_add(res, res, temp24);  // res = res + temp24
//    mpf_add(res, res, temp25);  // res = res + temp25
//    mpf_add(res, res, temp26);  // res = res + temp26
//    mpf_add(res, res, temp27);  // res = res + temp27
//    mpf_add(res, res, temp28);  // res = res + temp28
//    mpf_add(res, res, temp29);  // res = res + temp29
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//    mpf_clear(temp13);
//    mpf_clear(temp14);
//    mpf_clear(temp15);
//    mpf_clear(temp16);
//    mpf_clear(temp17);
//    mpf_clear(temp18);
//    mpf_clear(temp19);
//    mpf_clear(temp20);
//    mpf_clear(temp21);
//    mpf_clear(temp22);
//    mpf_clear(temp23);
//    mpf_clear(temp24);
//    mpf_clear(temp25);
//    mpf_clear(temp26);
//    mpf_clear(temp27);
//    mpf_clear(temp28);
//    mpf_clear(temp29);
//
//    return -result;  // Return the final result
//}
//double calculate_D(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10;
//    mpf_t temp11, temp12, temp13, temp14, temp15, temp16, temp17, temp18, temp19, temp20;
//    mpf_t temp21, temp22, temp23, temp24, temp25, temp26, temp27, temp28, temp29, temp30;
//    mpf_t temp31, temp32, temp33, temp34, temp35, temp36, temp37, temp38, temp39, temp40;;
//
//
//    mpf_init(temp1);
//    mpf_init(temp2);
//    mpf_init(temp3);
//    mpf_init(temp4);
//    mpf_init(temp5);
//    mpf_init(temp6);
//    mpf_init(temp7);
//    mpf_init(temp8);
//    mpf_init(temp9);
//    mpf_init(temp10);
//
//    mpf_init(temp11);
//    mpf_init(temp12);
//    mpf_init(temp13);
//    mpf_init(temp14);
//    mpf_init(temp15);
//    mpf_init(temp16);
//    mpf_init(temp17);
//    mpf_init(temp18);
//    mpf_init(temp19);
//    mpf_init(temp20);
//
//    mpf_init(temp21);
//    mpf_init(temp22);
//    mpf_init(temp23);
//    mpf_init(temp24);
//    mpf_init(temp25);
//    mpf_init(temp26);
//    mpf_init(temp27);
//    mpf_init(temp28);
//    mpf_init(temp29);
//    mpf_init(temp30);
//
//    mpf_init(temp31);
//    mpf_init(temp32);
//    mpf_init(temp33);
//    mpf_init(temp34);
//    mpf_init(temp35);
//    mpf_init(temp36);
//    mpf_init(temp37);
//    mpf_init(temp38);
//    mpf_init(temp39);
//    mpf_init(temp40);
//
//
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "0.01130372134364115060754327587799047340032", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//
//    mpf_set_str(temp2, "-176770.96143779244665226050442134242165157527552", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "-176.94773239923024339149460994860557489143808", 10);
//    mpf_mul(temp3, temp3, M);  // temp3 = temp3 * M
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//
//    mpf_set_str(temp4, "-179.37053440011290886042131980972317307043840", 10);
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, TR1);  // temp4 = temp4 * TR1
//
//    mpf_set_str(temp5, "-179.54730536155072104914711561853215705137152", 10);
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, TR2);  // temp5 = temp5 * TR2
//
//    mpf_set_str(temp6, "4652974.25610011569672731944346465281885369008128", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//
//    mpf_set_str(temp7, "-42010530.24657726931454402255062843824492402180096", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//
//    mpf_set_str(temp8, "6325555.63138143276307508604044905936477950574592", 10);
//    mpf_mul(temp8, temp8, M);  // temp8 = temp8 * M
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//
//    mpf_set_str(temp9, "-51025223.97642915191421683321950367572143879749632", 10);
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//
//    mpf_set_str(temp10, "10329.74961474354549005622405705894449796612096", 10);
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, TR1);  // temp10 = temp10 * TR1
//
//    mpf_set_str(temp11, "418039.07237296008461999752796079546326933569536", 10);
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, TR2);  // temp11 = temp11 * TR2
//
//    mpf_set_str(temp12, "-47164.48471371490019791004350555983703227498496", 10);
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, TR1);  // temp12 = temp12 * TR1
//
//    mpf_set_str(temp13, "-21487576.19206272780354115905636512824837462294528", 10);
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, TR2);  // temp13 = temp13 * TR2
//
//    mpf_set_str(temp14, "407710206613.02374846017593738533909861978651871936512", 10);
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//
//    mpf_set_str(temp15, "-23478958321140.09724709257790592017087107900272212443136", 10);
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//
//    mpf_set_str(temp16, "107202058536745.06086456415978275980716864993698512896000", 10);
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//    mpf_mul(temp16, temp16, T);  // temp16 = temp16 * T
//
//    mpf_set_str(temp17, "0.00001131502506498479101003352093765926912", 10);
//    mpf_mul(temp17, temp17, B);  // temp17 = temp17 * B
//    mpf_mul(temp17, temp17, M);  // temp17 = temp17 * M
//
//    mpf_set_str(temp18, "-26248.47838881785363919970910852840193794244608", 10);
//    mpf_mul(temp18, temp18, B);  // temp18 = temp18 * B
//    mpf_mul(temp18, temp18, T);  // temp18 = temp18 * T
//
//    mpf_set_str(temp19, "0.00001146995253987116711295197747089506304", 10);
//    mpf_mul(temp19, temp19, B);  // temp19 = temp19 * B
//    mpf_mul(temp19, temp19, TR1);  // temp19 = temp19 * TR1
//
//    mpf_set_str(temp20, "0.00001148125626121480743280080908034506752", 10);
//    mpf_mul(temp20, temp20, B);  // temp20 = temp20 * B
//    mpf_mul(temp20, temp20, TR2);  // temp20 = temp20 * TR2
//
//    mpf_set_str(temp21, "-0.40475732197571864672837639450473057484800", 10);
//    mpf_mul(temp21, temp21, B);  // temp21 = temp21 * B
//    mpf_mul(temp21, temp21, M);  // temp21 = temp21 * M
//    mpf_mul(temp21, temp21, T);  // temp21 = temp21 * T
//
//    mpf_set_str(temp22, "0.00000000017770114876988807125090902736896", 10);
//    mpf_mul(temp22, temp22, B);  // temp22 = temp22 * B
//    mpf_mul(temp22, temp22, M);  // temp22 = temp22 * M
//    mpf_mul(temp22, temp22, TR1);  // temp22 = temp22 * TR1
//
//    mpf_set_str(temp23, "0.00000000016639742742624693571011290857472", 10);
//    mpf_mul(temp23, temp23, B);  // temp23 = temp23 * B
//    mpf_mul(temp23, temp23, M);  // temp23 = temp23 * M
//    mpf_mul(temp23, temp23, TR2);  // temp23 = temp23 * TR2
//
//    mpf_set_str(temp24, "-0.00204291709559486522535675718158367326208", 10);
//    mpf_mul(temp24, temp24, B);  // temp24 = temp24 * B
//    mpf_mul(temp24, temp24, T);  // temp24 = temp24 * T
//    mpf_mul(temp24, temp24, TR1);  // temp24 = temp24 * TR1
//
//    mpf_set_str(temp25, "-0.02793595796000667400080707823118864875520", 10);
//    mpf_mul(temp25, temp25, B);  // temp25 = temp25 * B
//    mpf_mul(temp25, temp25, T);  // temp25 = temp25 * T
//    mpf_mul(temp25, temp25, TR2);  // temp25 = temp25 * TR2
//
//    mpf_set_str(temp26, "0.00000000001146995253987116755547567685632", 10);
//    mpf_mul(temp26, temp26, B);  // temp26 = temp26 * B
//    mpf_mul(temp26, temp26, TR1);  // temp26 = temp26 * TR1
//    mpf_mul(temp26, temp26, TR2);  // temp26 = temp26 * TR2
//
//    mpf_set_str(temp27, "-0.00277894349672059002745290402357315633152", 10);
//    mpf_mul(temp27, temp27, M);  // temp27 = temp27 * M
//    mpf_mul(temp27, temp27, T);  // temp27 = temp27 * T
//    mpf_mul(temp27, temp27, TR1);  // temp27 = temp27 * TR1
//
//    mpf_set_str(temp28, "-0.00260217253528279740216918412850079203328", 10);
//    mpf_mul(temp28, temp28, M);  // temp28 = temp28 * M
//    mpf_mul(temp28, temp28, T);  // temp28 = temp28 * T
//    mpf_mul(temp28, temp28, TR2);  // temp28 = temp28 * TR2
//
//    mpf_set_str(temp29, "-0.00017937053440011293006195788130128232448", 10);
//    mpf_mul(temp29, temp29, T);  // temp29 = temp29 * T
//    mpf_mul(temp29, temp29, TR1);  // temp29 = temp29 * TR1
//    mpf_mul(temp29, temp29, TR2);  // temp29 = temp29 * TR2
//
//    mpf_set_str(temp30, "3.86818963268595072133291275884641107574784", 10);
//    mpf_mul(temp30, temp30, B);  // temp30 = temp30 * B
//    mpf_mul(temp30, temp30, M);  // temp30 = temp30 * M
//    mpf_mul(temp30, temp30, T);  // temp30 = temp30 * T
//    mpf_mul(temp30, temp30, T);  // temp30 = temp30 * T
//
//    mpf_set_str(temp31, "0.01846388093521996180115108102950195560448", 10);
//    mpf_mul(temp31, temp31, B);  // temp31 = temp31 * B
//    mpf_mul(temp31, temp31, T);  // temp31 = temp31 * T
//    mpf_mul(temp31, temp31, T);  // temp31 = temp31 * T
//    mpf_mul(temp31, temp31, TR1);  // temp31 = temp31 * TR1
//
//    mpf_set_str(temp32, "4.11442562079591658212110268253557902802944", 10);
//    mpf_mul(temp32, temp32, B);  // temp32 = temp32 * B
//    mpf_mul(temp32, temp32, T);  // temp32 = temp32 * T
//    mpf_mul(temp32, temp32, T);  // temp32 = temp32 * T
//    mpf_mul(temp32, temp32, TR2);  // temp32 = temp32 * TR2
//
//    mpf_set_str(temp33, "0.02243076175434536536429345425399983636480", 10);
//    mpf_mul(temp33, temp33, M);  // temp33 = temp33 * M
//    mpf_mul(temp33, temp33, T);  // temp33 = temp33 * T
//    mpf_mul(temp33, temp33, T);  // temp33 = temp33 * T
//    mpf_mul(temp33, temp33, TR1);  // temp33 = temp33 * TR1
//
//    mpf_set_str(temp34, "5.92170904575866859795197328144624095592448", 10);
//    mpf_mul(temp34, temp34, M);  // temp34 = temp34 * M
//    mpf_mul(temp34, temp34, T);  // temp34 = temp34 * T
//    mpf_mul(temp34, temp34, T);  // temp34 = temp34 * T
//    mpf_mul(temp34, temp34, TR2);  // temp34 = temp34 * TR2
//
//
//    mpf_set_str(temp35, "0.00943289694274297973566772905655110991872", 10);
//    mpf_mul(temp35, temp35, T);  // temp35 = temp35 * T
//    mpf_mul(temp35, temp35, T);  // temp35 = temp35 * T
//    mpf_mul(temp35, temp35, TR1);  // temp35 = temp35 * TR1
//    mpf_mul(temp35, temp35, TR2);  // temp35 = temp35 * TR2
//
//    mpf_set_str(temp36, "-0.00000000170009177962517313516538236502016", 10);
//    mpf_mul(temp36, temp36, B);  // temp36 = temp36 * B
//    mpf_mul(temp36, temp36, M);  // temp36 = temp36 * M
//    mpf_mul(temp36, temp36, T);  // temp36 = temp36 * T
//    mpf_mul(temp36, temp36, TR1);  // temp36 = temp36 * TR1
//
//    mpf_set_str(temp37, "-0.00000037866710904851354563064186697416704", 10);
//    mpf_mul(temp37, temp37, B);  // temp37 = temp37 * B
//    mpf_mul(temp37, temp37, M);  // temp37 = temp37 * M
//    mpf_mul(temp37, temp37, T);  // temp37 = temp37 * T
//    mpf_mul(temp37, temp37, TR2);  // temp37 = temp37 * TR2
//
//    mpf_set_str(temp38, "0.00000000000000016623119623001691580792832", 10);
//    mpf_mul(temp38, temp38, B);  // temp38 = temp38 * B
//    mpf_mul(temp38, temp38, M);  // temp38 = temp38 * M
//    mpf_mul(temp38, temp38, TR1);  // temp38 = temp38 * TR1
//    mpf_mul(temp38, temp38, TR2);  // temp38 = temp38 * TR2
//
//    mpf_set_str(temp39, "-0.00000000180619679849539623502563816505344", 10);
//    mpf_mul(temp39, temp39, B);  // temp39 = temp39 * B
//    mpf_mul(temp39, temp39, T);  // temp39 = temp39 * T
//    mpf_mul(temp39, temp39, TR1);  // temp39 = temp39 * TR1
//    mpf_mul(temp39, temp39, TR2);  // temp39 = temp39 * TR2
//
//    mpf_set_str(temp40, "-0.00000000259957296232047693535935788482560", 10);
//    mpf_mul(temp40, temp40, M);  // temp40 = temp40 * M
//    mpf_mul(temp40, temp40, T);  // temp40 = temp40 * T
//    mpf_mul(temp40, temp40, TR1);  // temp40 = temp40 * TR1
//    mpf_mul(temp40, temp40, TR2);  // temp40 = temp40 * TR2
//
//
//
//
//
//
//
//
//    // Add all terms to res
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);    // res = res + temp3
//    mpf_add(res, res, temp4);    // res = res + temp4
//    mpf_add(res, res, temp5);    // res = res + temp5
//    mpf_add(res, res, temp6);    // res = res + temp6
//    mpf_add(res, res, temp7);    // res = res + temp7
//    mpf_add(res, res, temp8);    // res = res + temp8
//    mpf_add(res, res, temp9);    // res = res + temp9
//    mpf_add(res, res, temp10);   // res = res + temp10
//    mpf_add(res, res, temp11);   // res = res + temp11
//    mpf_add(res, res, temp12);   // res = res + temp12
//    mpf_add(res, res, temp13);   // res = res + temp13
//    mpf_add(res, res, temp14);   // res = res + temp14
//    mpf_add(res, res, temp15);   // res = res + temp15
//    mpf_add(res, res, temp16);   // res = res + temp16
//    mpf_add(res, res, temp17);   // res = res + temp17
//    mpf_add(res, res, temp18);   // res = res + temp18
//    mpf_add(res, res, temp19);   // res = res + temp19
//    mpf_add(res, res, temp20);   // res = res + temp20
//    mpf_add(res, res, temp21);   // res = res + temp21
//    mpf_add(res, res, temp22);   // res = res + temp22
//    mpf_add(res, res, temp23);   // res = res + temp23
//    mpf_add(res, res, temp24);   // res = res + temp24
//    mpf_add(res, res, temp25);   // res = res + temp25
//    mpf_add(res, res, temp26);   // res = res + temp26
//    mpf_add(res, res, temp27);   // res = res + temp27
//    mpf_add(res, res, temp28);   // res = res + temp28
//    mpf_add(res, res, temp29);   // res = res + temp29
//    mpf_add(res, res, temp30);   // res = res + temp30
//    mpf_add(res, res, temp31);   // res = res + temp31
//    mpf_add(res, res, temp32);   // res = res + temp32
//    mpf_add(res, res, temp33);   // res = res + temp33
//    mpf_add(res, res, temp34);   // res = res + temp34
//    mpf_add(res, res, temp35);   // res = res + temp35
//    mpf_add(res, res, temp36);   // res = res + temp36
//    mpf_add(res, res, temp37);   // res = res + temp37
//    mpf_add(res, res, temp38);   // res = res + temp38
//    mpf_add(res, res, temp39);   // res = res + temp39
//    mpf_add(res, res, temp40);   // res = res + temp40
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);   // Clear temp1
//    mpf_clear(temp2);   // Clear temp2
//    mpf_clear(temp3);   // Clear temp3
//    mpf_clear(temp4);   // Clear temp4
//    mpf_clear(temp5);   // Clear temp5
//    mpf_clear(temp6);   // Clear temp6
//    mpf_clear(temp7);   // Clear temp7
//    mpf_clear(temp8);   // Clear temp8
//    mpf_clear(temp9);   // Clear temp9
//    mpf_clear(temp10);  // Clear temp10
//    mpf_clear(temp11);  // Clear temp11
//    mpf_clear(temp12);  // Clear temp12
//    mpf_clear(temp13);  // Clear temp13
//    mpf_clear(temp14);  // Clear temp14
//    mpf_clear(temp15);  // Clear temp15
//    mpf_clear(temp16);  // Clear temp16
//    mpf_clear(temp17);  // Clear temp17
//    mpf_clear(temp18);  // Clear temp18
//    mpf_clear(temp19);  // Clear temp19
//    mpf_clear(temp20);  // Clear temp20
//    mpf_clear(temp21);  // Clear temp21
//    mpf_clear(temp22);  // Clear temp22
//    mpf_clear(temp23);  // Clear temp23
//    mpf_clear(temp24);  // Clear temp24
//    mpf_clear(temp25);  // Clear temp25
//    mpf_clear(temp26);  // Clear temp26
//    mpf_clear(temp27);  // Clear temp27
//    mpf_clear(temp28);  // Clear temp28
//    mpf_clear(temp29);  // Clear temp29
//    mpf_clear(temp30);  // Clear temp30
//    mpf_clear(temp31);  // Clear temp31
//    mpf_clear(temp32);  // Clear temp32
//    mpf_clear(temp33);  // Clear temp33
//    mpf_clear(temp34);  // Clear temp34
//    mpf_clear(temp35);  // Clear temp35
//    mpf_clear(temp36);  // Clear temp36
//    mpf_clear(temp37);  // Clear temp37
//    mpf_clear(temp38);  // Clear temp38
//    mpf_clear(temp39);  // Clear temp39
//    mpf_clear(temp40);  // Clear temp40
//
//    return -result;  // Return the final result
//}
//
//double calculate_E(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13,
//        temp14, temp15;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7); mpf_init(temp8); mpf_init(temp9); mpf_init(temp10); mpf_init(temp11); mpf_init(temp12);
//    mpf_init(temp13); mpf_init(temp14); mpf_init(temp15);
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "4027971.58756583523155850680214954774953359048704", 10);  // temp1 = 4027971.58756583523155850680214954774953359048704
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T (B * T)
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T (B * T * T)
//
//    mpf_set_str(temp2, "5916962.45381602729876936696061507665516944162816", 10);  // temp2 = 5916962.45381602729876936696061507665516944162816
//    mpf_mul(temp2, temp2, M);  // temp2 = temp2 * M
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T (M * T)
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T (M * T * T)
//
//    mpf_set_str(temp3, "8842.08490585196215872031818855878091469750272", 10);  // temp3 = 8842.08490585196215872031818855878091469750272
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T (T * T)
//    mpf_mul(temp3, temp3, TR2);  // temp3 = temp3 * TR2 (T * T * TR2)
//
//    mpf_set_str(temp4, "0.00001130372134364115056986896299372642304", 10);  // temp4 = 0.00001130372134364115056986896299372642304
//    mpf_mul(temp4, temp4, B);  // temp4 = temp4 * B
//    mpf_mul(temp4, temp4, M);  // temp4 = temp4 * M (B * M)
//
//    mpf_set_str(temp5, "176.77096143779245537939396284922675731628032", 10);  // temp5 = 176.77096143779245537939396284922675731628032
//    mpf_mul(temp5, temp5, B);  // temp5 = temp5 * B
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T (B * T)
//
//    mpf_set_str(temp6, "0.00001130372134364115056986896299372642304", 10);  // temp6 = 0.00001130372134364115056986896299372642304
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, TR2);  // temp6 = temp6 * TR2 (B * TR2)
//
//    mpf_set_str(temp7, "176.77096143779245537939396284922675731628032", 10);  // temp7 = 176.77096143779245537939396284922675731628032
//    mpf_mul(temp7, temp7, M);  // temp7 = temp7 * M
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T (M * T)
//
//    mpf_set_str(temp8, "176.77096143779245537939396284922675731628032", 10);  // temp8 = 176.77096143779245537939396284922675731628032
//    mpf_mul(temp8, temp8, T);  // temp8 = temp8 * T
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2 (T * TR2)
//
//    mpf_set_str(temp9, "0.37862932923118854689210246793471159435264", 10);  // temp9 = 0.37862932923118854689210246793471159435264
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M (B * M)
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T (B * M * T)
//
//    mpf_set_str(temp10, "0.00000000016623119623001691852489561337845", 10);  // temp11 = 0.00000000016623119623001691852489561337845
//    mpf_mul(temp10, temp10, B);  // temp11 = temp11 * B
//    mpf_mul(temp10, temp10, M);  // temp11 = temp11 * M (B * M)
//    mpf_mul(temp10, temp10, TR1);  // temp11 = temp11 * TR1 (B * M * TR1)
//
//    mpf_set_str(temp11, "0.00000000016623119623001691852489561337845", 10);  // temp12 = 0.00000000016623119623001691852489561337845
//    mpf_mul(temp11, temp11, B);  // temp12 = temp12 * B
//    mpf_mul(temp11, temp11, M);  // temp12 = temp12 * M (B * M)
//    mpf_mul(temp11, temp11, TR2);  // temp12 = temp12 * TR2 (B * M * TR2)
//
//    mpf_set_str(temp12, "0.00176841698117039259279305097898597810176", 10);  // temp13 = 0.00176841698117039259279305097898597810176
//    mpf_mul(temp12, temp12, B);  // temp13 = temp13 * B
//    mpf_mul(temp12, temp12, T);  // temp13 = temp13 * T (B * T)
//    mpf_mul(temp12, temp12, TR1);  // temp13 = temp13 * TR1 (B * T * TR1)
//
//    mpf_set_str(temp13, "0.00176841698117039259279305097898597810176", 10);  // temp14 = 0.00176841698117039259279305097898597810176
//    mpf_mul(temp13, temp13, B);  // temp14 = temp14 * B
//    mpf_mul(temp13, temp13, T);  // temp14 = temp14 * T (B * T)
//    mpf_mul(temp13, temp13, TR2);  // temp14 = temp14 * TR2 (B * T * TR2)
//
//    mpf_set_str(temp14, "0.00259957296232047714672908623097433686016", 10);  // temp15 = 0.00259957296232047714672908623097433686016
//    mpf_mul(temp14, temp14, M);  // temp15 = temp15 * M
//    mpf_mul(temp14, temp14, T);  // temp15 = temp15 * T (M * T)
//    mpf_mul(temp14, temp14, TR1);  // temp15 = temp15 * TR1 (M * T * TR1)
//
//    mpf_set_str(temp15, "0.00259957296232047714672908623097433686016", 10);  // temp16 = 0.00259957296232047714672908623097433686016
//    mpf_mul(temp15, temp15, M);  // temp16 = temp16 * M
//    mpf_mul(temp15, temp15, T);  // temp16 = temp16 * T (M * T)
//    mpf_mul(temp15, temp15, TR2);  // temp16 = temp16 * TR2 (M * T * TR2)
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//    mpf_add(res, res, temp13);  // res = res + temp13
//    mpf_add(res, res, temp14);  // res = res + temp14
//    mpf_add(res, res, temp15);  // res = res + temp15
//
//
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//    mpf_clear(temp13);
//    mpf_clear(temp14);
//    mpf_clear(temp15);
//
//
//    return result;  // Return the final result
//}
//double calculate_F(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7); mpf_init(temp8); mpf_init(temp9); mpf_init(temp10); mpf_init(temp11); mpf_init(temp12);
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "-0.00004521488537456460227947585197490569216", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//    mpf_mul(temp1, temp1, M);  // temp1 = temp1 * M
//
//    mpf_set_str(temp2, "-353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp2, temp2, B);  // temp2 = temp2 * B
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "-0.00004521488537456460227947585197490569216", 10);
//    mpf_mul(temp3, temp3, B);  // temp3 = temp3 * B
//    mpf_mul(temp3, temp3, TR2);  // temp3 = temp3 * TR2
//
//    mpf_set_str(temp4, "-353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp4, temp4, M);  // temp4 = temp4 * M
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//
//    mpf_set_str(temp5, "-353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, TR2);  // temp5 = temp5 * TR2
//
//    mpf_set_str(temp6, "-0.75725865846237709378420493586942318870528", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, M);  // temp6 = temp6 * M
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//
//    mpf_set_str(temp7, "-0.00000000066492478492006767409958245351380", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, M);  // temp7 = temp7 * M
//    mpf_mul(temp7, temp7, TR1);  // temp7 = temp7 * TR1
//
//    mpf_set_str(temp8, "-0.00000000066492478492006767409958245351380", 10);
//    mpf_mul(temp8, temp8, B);  // temp8 = temp8 * B
//    mpf_mul(temp8, temp8, M);  // temp8 = temp8 * M
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2
//
//    mpf_set_str(temp9, "-0.00353683396234078518558610195797195620352", 10);
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//    mpf_mul(temp9, temp9, TR1);  // temp9 = temp9 * TR1
//
//    mpf_set_str(temp10, "-0.00353683396234078518558610195797195620352", 10);
//    mpf_mul(temp10, temp10, B);  // temp10 = temp10 * B
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, TR2);  // temp10 = temp10 * TR2
//
//    mpf_set_str(temp11, "-0.00519914592464095429345817246194867372032", 10);
//    mpf_mul(temp11, temp11, M);  // temp11 = temp11 * M
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, TR1);  // temp11 = temp11 * TR1
//
//    mpf_set_str(temp12, "-0.00519914592464095429345817246194867372032", 10);
//    mpf_mul(temp12, temp12, M);  // temp12 = temp12 * M
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, TR2);  // temp12 = temp12 * TR2
//
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//
//
//
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//
//
//
//    return result;  // Return the final result
//}
//double calculate_G(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7);
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "-8055943.17513167046311701360429909549906718097408", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T
//
//    mpf_set_str(temp2, "-11833924.90763205459753873392123015331033888325632", 10);
//    mpf_mul(temp2, temp2, M);  // temp2 = temp2 * M
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "-17684.16981170392431744063637711756182939500544", 10);
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//    mpf_mul(temp3, temp3, TR2);  // temp3 = temp3 * TR2
//
//    mpf_set_str(temp4, "0.00006782232806184690341921377796235853824", 10);
//    mpf_mul(temp4, temp4, B);  // temp4 = temp4 * B
//    mpf_mul(temp4, temp4, M);  // temp4 = temp4 * M
//
//    mpf_set_str(temp5, "0.00006782232806184690341921377796235853824", 10);
//    mpf_mul(temp5, temp5, B);  // temp5 = temp5 * B
//    mpf_mul(temp5, temp5, TR2);  // temp5 = temp5 * TR2
//
//    mpf_set_str(temp6, "0.00000000099738717738010151114937368027070", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, M);  // temp6 = temp6 * M
//    mpf_mul(temp6, temp6, TR1);  // temp6 = temp6 * TR1
//
//    mpf_set_str(temp7, "0.00000000099738717738010151114937368027070", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, M);  // temp7 = temp7 * M
//    mpf_mul(temp7, temp7, TR2);  // temp7 = temp7 * TR2
//
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//
//
//
//
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//
//
//
//
//    return result;  // Return the final result
//}
//double calculate_H(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7); mpf_init(temp8); mpf_init(temp9); mpf_init(temp10); mpf_init(temp11); mpf_init(temp12);
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "-0.00004521488537456460227947585197490569216", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//    mpf_mul(temp1, temp1, M);  // temp1 = temp1 * M
//
//    mpf_set_str(temp2, "353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp2, temp2, B);  // temp2 = temp2 * B
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//
//    mpf_set_str(temp3, "-0.00004521488537456460227947585197490569216", 10);
//    mpf_mul(temp3, temp3, B);  // temp3 = temp3 * B
//    mpf_mul(temp3, temp3, TR2);  // temp3 = temp3 * TR2
//
//    mpf_set_str(temp4, "353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp4, temp4, M);  // temp4 = temp4 * M
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//
//    mpf_set_str(temp5, "353.54192287558491075878792569845351463256064", 10);
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, TR2);  // temp5 = temp5 * TR2
//
//    mpf_set_str(temp6, "0.75725865846237709378420493586942318870528", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, M);  // temp6 = temp6 * M
//    mpf_mul(temp6, temp6, T);  // temp6 = temp6 * T
//
//    mpf_set_str(temp7, "-0.00000000066492478492006767409958245351380", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, M);  // temp7 = temp7 * M
//    mpf_mul(temp7, temp7, TR1);  // temp7 = temp7 * TR1
//
//    mpf_set_str(temp8, "-0.00000000066492478492006767409958245351380", 10);
//    mpf_mul(temp8, temp8, B);  // temp8 = temp8 * B
//    mpf_mul(temp8, temp8, M);  // temp8 = temp8 * M
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2
//
//    mpf_set_str(temp9, "0.00353683396234078518558610195797195620352", 10);
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//    mpf_mul(temp9, temp9, TR1);  // temp9 = temp9 * TR1
//
//    mpf_set_str(temp10, "0.00353683396234078518558610195797195620352", 10);
//    mpf_mul(temp10, temp10, B);  // temp10 = temp10 * B
//    mpf_mul(temp10, temp10, T);  // temp10 = temp10 * T
//    mpf_mul(temp10, temp10, TR2);  // temp10 = temp10 * TR2
//
//    mpf_set_str(temp11, "0.00519914592464095429345817246194867372032", 10);
//    mpf_mul(temp11, temp11, M);  // temp11 = temp11 * M
//    mpf_mul(temp11, temp11, T);  // temp11 = temp11 * T
//    mpf_mul(temp11, temp11, TR1);  // temp11 = temp11 * TR1
//
//    mpf_set_str(temp12, "0.00519914592464095429345817246194867372032", 10);
//    mpf_mul(temp12, temp12, M);  // temp12 = temp12 * M
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, TR2);  // temp12 = temp12 * TR2
//
//
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//
//
//
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//
//
//
//    return result;  // Return the final result
//}
//double calculate_I(const double& B_val, const double& T_val, const double& M_val, const double& TR1_val, const double& TR2_val, const mpf_t& G) {
//    mpf_t B, T, M, TR1, TR2, res;
//
//    // Initialize the variables for B, T, M, TR1, TR2, and result (res)
//    mpf_init(B);
//    mpf_init(T);
//    mpf_init(M);
//    mpf_init(TR1);
//    mpf_init(TR2);
//    mpf_init(res);
//
//    // Set the values for B, T, M, TR1, TR2 from the input doubles
//    mpf_set_d(B, B_val);  // Set B using input double B_val
//    mpf_set_d(T, T_val);  // Set T using input double T_val
//    mpf_set_d(M, M_val);  // Set M using input double M_val
//    mpf_set_d(TR1, TR1_val);  // Set TR1 using input double TR1_val
//    mpf_set_d(TR2, TR2_val);  // Set TR2 using input double TR2_val
//
//    // Initialize temporary variables (temp1 to temp29) for intermediate calculations
//    mpf_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13,
//        temp14, temp15;
//
//    mpf_init(temp1); mpf_init(temp2); mpf_init(temp3); mpf_init(temp4); mpf_init(temp5); mpf_init(temp6);
//    mpf_init(temp7); mpf_init(temp8); mpf_init(temp9); mpf_init(temp10); mpf_init(temp11); mpf_init(temp12);
//    mpf_init(temp13); mpf_init(temp14); mpf_init(temp15);
//
//    // Set constants for the calculation using string initializations
//    mpf_set_str(temp1, "4027971.58756583523155850680214954774953359048704", 10);
//    mpf_mul(temp1, temp1, B);  // temp1 = temp1 * B
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T
//    mpf_mul(temp1, temp1, T);  // temp1 = temp1 * T
//
//    mpf_set_str(temp2, "-176.77096143779245537939396284922675731628032", 10);
//    mpf_mul(temp2, temp2, T);  // temp2 = temp2 * T
//    mpf_mul(temp2, temp2, TR2);  // temp2 = temp2 * TR2
//
//    mpf_set_str(temp3, "-176.77096143779245537939396284922675731628032", 10);
//    mpf_mul(temp3, temp3, M);  // temp3 = temp3 * M
//    mpf_mul(temp3, temp3, T);  // temp3 = temp3 * T
//
//    mpf_set_str(temp4, "5916962.45381602729876936696061507665516944162816", 10);
//    mpf_mul(temp4, temp4, M);  // temp4 = temp4 * M
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//    mpf_mul(temp4, temp4, T);  // temp4 = temp4 * T
//
//    mpf_set_str(temp5, "8842.08490585196215872031818855878091469750272", 10);
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, T);  // temp5 = temp5 * T
//    mpf_mul(temp5, temp5, TR2);  // temp5 = temp5 * TR2
//
//    mpf_set_str(temp6, "0.00001130372134364115056986896299372642304", 10);
//    mpf_mul(temp6, temp6, B);  // temp6 = temp6 * B
//    mpf_mul(temp6, temp6, M);  // temp6 = temp6 * M
//
//    mpf_set_str(temp7, "-176.77096143779245537939396284922675731628032", 10);
//    mpf_mul(temp7, temp7, B);  // temp7 = temp7 * B
//    mpf_mul(temp7, temp7, T);  // temp7 = temp7 * T
//
//    mpf_set_str(temp8, "0.00001130372134364115056986896299372642304", 10);
//    mpf_mul(temp8, temp8, B);  // temp8 = temp8 * B
//    mpf_mul(temp8, temp8, TR2);  // temp8 = temp8 * TR2
//
//    mpf_set_str(temp9, "-0.37862932923118854689210246793471159435264", 10);
//    mpf_mul(temp9, temp9, B);  // temp9 = temp9 * B
//    mpf_mul(temp9, temp9, M);  // temp9 = temp9 * M
//    mpf_mul(temp9, temp9, T);  // temp9 = temp9 * T
//
//    mpf_set_str(temp10, "0.00000000016623119623001691852489561337845", 10);
//    mpf_mul(temp10, temp10, B);  // temp10 = temp10 * B
//    mpf_mul(temp10, temp10, M);  // temp10 = temp10 * M
//    mpf_mul(temp10, temp10, TR1);  // temp10 = temp10 * TR1
//
//    mpf_set_str(temp11, "0.00000000016623119623001691852489561337845", 10);
//    mpf_mul(temp11, temp11, B);  // temp11 = temp11 * B
//    mpf_mul(temp11, temp11, M);  // temp11 = temp11 * M
//    mpf_mul(temp11, temp11, TR2);  // temp11 = temp11 * TR2
//
//    mpf_set_str(temp12, "-0.00176841698117039259279305097898597810176", 10);
//    mpf_mul(temp12, temp12, B);  // temp12 = temp12 * B
//    mpf_mul(temp12, temp12, T);  // temp12 = temp12 * T
//    mpf_mul(temp12, temp12, TR1);  // temp12 = temp12 * TR1
//
//    mpf_set_str(temp13, "-0.00176841698117039259279305097898597810176", 10);
//    mpf_mul(temp13, temp13, B);  // temp13 = temp13 * B
//    mpf_mul(temp13, temp13, T);  // temp13 = temp13 * T
//    mpf_mul(temp13, temp13, TR2);  // temp13 = temp13 * TR2
//
//    mpf_set_str(temp14, "-0.00259957296232047714672908623097433686016", 10);
//    mpf_mul(temp14, temp14, M);  // temp14 = temp14 * M
//    mpf_mul(temp14, temp14, T);  // temp14 = temp14 * T
//    mpf_mul(temp14, temp14, TR1);  // temp14 = temp14 * TR1
//
//    mpf_set_str(temp15, "-0.00259957296232047714672908623097433686016", 10);
//    mpf_mul(temp15, temp15, M);  // temp15 = temp15 * M
//    mpf_mul(temp15, temp15, T);  // temp15 = temp15 * T
//    mpf_mul(temp15, temp15, TR2);  // temp15 = temp15 * TR2
//
//
//    // Add all the temporary variables
//    mpf_add(res, temp1, temp2);  // res = temp1 + temp2
//    mpf_add(res, res, temp3);  // res = res + temp3
//    mpf_add(res, res, temp4);  // res = res + temp4
//    mpf_add(res, res, temp5);  // res = res + temp5
//    mpf_add(res, res, temp6);  // res = res + temp6
//    mpf_add(res, res, temp7);  // res = res + temp7
//    mpf_add(res, res, temp8);  // res = res + temp8
//    mpf_add(res, res, temp9);  // res = res + temp9
//    mpf_add(res, res, temp10);  // res = res + temp10
//    mpf_add(res, res, temp11);  // res = res + temp11
//    mpf_add(res, res, temp12);  // res = res + temp12
//    mpf_add(res, res, temp13);  // res = res + temp13
//    mpf_add(res, res, temp14);  // res = res + temp14
//    mpf_add(res, res, temp15);  // res = res + temp15
//
//
//
//
//    // Finally, multiply the result by G
//    mpf_mul(res, res, G);  // res = res * G
//
//    // Convert the final result from mpf_t to a double for the return value
//    double result = mpf_get_d(res);
//
//    // Clear all initialized variables to free memory
//    mpf_clear(B);
//    mpf_clear(T);
//    mpf_clear(M);
//    mpf_clear(TR1);
//    mpf_clear(TR2);
//    mpf_clear(res);
//
//    mpf_clear(temp1);
//    mpf_clear(temp2);
//    mpf_clear(temp3);
//    mpf_clear(temp4);
//    mpf_clear(temp5);
//    mpf_clear(temp6);
//    mpf_clear(temp7);
//    mpf_clear(temp8);
//    mpf_clear(temp9);
//    mpf_clear(temp10);
//    mpf_clear(temp11);
//    mpf_clear(temp12);
//    mpf_clear(temp13);
//    mpf_clear(temp14);
//    mpf_clear(temp15);
//
//
//    return result;  // Return the final result
//}
