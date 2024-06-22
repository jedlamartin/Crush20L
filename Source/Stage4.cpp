#include "Stage4.h"
#include <cmath>

void Stage4::processBlock(juce::AudioBuffer<float>& buffer) {
    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, CircularBuffer<float, 4>{0.0f});
    this->uBuffer.resize(channelNumber, CircularBuffer<float, 4>{0.0f});


    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    for (int channel = 0; channel < channelNumber; channel++) {
        auto channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float tmp = channelSamples[i];
            /*float test = this->A * this->yBuffer[channel][0] + this->B * this->yBuffer[channel][1] + this->C * this->yBuffer[channel][2] + this->D * this->yBuffer[channel][3] + this->E * channelSamples[i] + this->F * this->uBuffer[channel][0] + this->G * this->uBuffer[channel][1] + this->H * this->uBuffer[channel][2] + this->I * this->uBuffer[channel][3];
            if (test != test || isinf(test) || test>10 || test<-10) {
                jassertfalse;
            }*/
            channelSamples[i] = this->A * this->yBuffer[channel][0] + this->B * this->yBuffer[channel][1] + this->C * this->yBuffer[channel][2] + this->D * this->yBuffer[channel][3] + this->E * channelSamples[i] + this->F * this->uBuffer[channel][0] + this->G * this->uBuffer[channel][1] + this->H * this->uBuffer[channel][2] + this->I * this->uBuffer[channel][3];
            
            this->yBuffer[channel].push(channelSamples[i]);
            this->uBuffer[channel].push(tmp);
        }
    }
}

void Stage4::configure(double sampleRate) {

    this->sampleRate = static_cast<float>(sampleRate);
    double T = 1 / sampleRate;
    double B = static_cast<double>(*this->bassParameter);
    double M = static_cast<double>(*this->midParameter);
    double TR1 = static_cast<double>(*this->trebleParameter);
    double TR2 = 250000.0 + 0.01 - TR1;

    double G = 1 / (+0.01130372134364115060754327587799047340032 * B + 176770.96143779244665226050442134242165157527552 * T + 407710206613.02374846017593738533909861978651871936512 * T * T + 23478958321140.09724709257790592017087107900272212443136 * T * T * T + 107202058536745.06086456415978275980716864993698512896000 * T * T * T * T + 4652974.25610011569672731944346465281885369008128 * B * T * T + 42010530.24657726931454402255062843824492402180096 * B * T * T * T + 6325555.63138143276307508604044905936477950574592 * M * T * T + 51025223.97642915191421683321950367572143879749632 * M * T * T * T + 10329.74961474354549005622405705894449796612096 * T * T * TR1 + 418039.07237296008461999752796079546326933569536 * T * T * TR2 + 47164.48471371490019791004350555983703227498496 * T * T * T * TR1 + 21487576.19206272780354115905636512824837462294528 * T * T * T * TR2 + 0.00001131502506498479101003352093765926912 * B * M + 26248.47838881785363919970910852840193794244608 * B * T + 0.00001146995253987116711295197747089506304 * B * TR1 + 0.00001148125626121480743280080908034506752 * B * TR2 + 176.94773239923024339149460994860557489143808 * M * T + 179.37053440011290886042131980972317307043840 * T * TR1 + 179.54730536155072104914711561853215705137152 * T * TR2 + 0.40475732197571864672837639450473057484800 * B * M * T + 0.00000000017770114876988807125090902736896 * B * M * TR1 + 0.00000000016639742742624693571011290857472 * B * M * TR2 + 0.00204291709559486522535675718158367326208 * B * T * TR1 + 0.02793595796000667400080707823118864875520 * B * T * TR2 + 0.00000000001146995253987116755547567685632 * B * TR1 * TR2 + 0.00277894349672059002745290402357315633152 * M * T * TR1 + 0.00260217253528279740216918412850079203328 * M * T * TR2 + 0.00017937053440011293006195788130128232448 * T * TR1 * TR2 + 3.86818963268595072133291275884641107574784 * B * M * T * T + 0.01846388093521996180115108102950195560448 * B * T * T * TR1 + 4.11442562079591658212110268253557902802944 * B * T * T * TR2 + 0.02243076175434536536429345425399983636480 * M * T * T * TR1 + 5.92170904575866859795197328144624095592448 * M * T * T * TR2 + 0.00943289694274297973566772905655110991872 * T * T * TR1 * TR2 + 0.00000000170009177962517313516538236502016 * B * M * T * TR1 + 0.00000037866710904851354563064186697416704 * B * M * T * TR2 + 0.00000000000000016623119623001691580792832 * B * M * TR1 * TR2 + 0.00000000180619679849539623502563816505344 * B * T * TR1 * TR2 + 0.00000000259957296232047693535935788482560 * M * T * TR1 * TR2);
    this->A = -(-0.04521488537456460243017310351196189360128 * B - 353541.92287558489330452100884268484330315055104 * T + 46957916642280.19449418515581184034174215800544424886272 * T * T * T + 428808234146980.24345825663913103922867459974794051584000 * T * T * T * T + 84021060.49315453862908804510125687648984804360192 * B * T * T * T + 102050447.95285830382843366643900735144287759499264 * M * T * T * T + 94328.96942742980039582008701111967406454996992 * T * T * T * TR1 + 42975152.38412545560708231811273025649674924589056 * T * T * T * TR2 - 0.00004526010025993916404013408375063707648 * B * M - 52496.95677763570727839941821705680387588489216 * B * T - 0.00004587981015948466845180790988358025216 * B * TR1 - 0.00004592502504485922973120323632138027008 * B * TR2 - 353.89546479846048678298921989721114978287616 * M * T - 358.74106880022581772084263961944634614087680 * T * TR1 - 359.09461072310144209829423123706431410274304 * T * TR2 - 0.80951464395143729345675278900946114969600 * B * M * T - 0.00000000071080459507955228500363610947584 * B * M * TR1 - 0.00000000066558970970498774284045163429888 * B * M * TR2 - 0.00408583419118973045071351436316734652416 * B * T * TR1 - 0.05587191592001334800161415646237729751040 * B * T * TR2 - 0.00000000004587981015948467022190270742528 * B * TR1 * TR2 - 0.00555788699344118005490580804714631266304 * M * T * TR1 - 0.00520434507056559480433836825700158406656 * M * T * TR2 - 0.00035874106880022586012391576260256464896 * T * TR1 * TR2 - 0.00000000340018355925034627033076473004032 * B * M * T * TR1 - 0.00000075733421809702709126128373394833408 * B * M * T * TR2 - 0.00000000000000066492478492006766323171328 * B * M * TR1 * TR2 - 0.00000000361239359699079247005127633010688 * B * T * TR1 * TR2 - 0.00000000519914592464095387071871576965120 * M * T * TR1 * TR2) * G;
    this->B = -(+0.06782232806184690364525965526794284040192 * B - 815420413226.04749692035187477067819723957303743873024 * T * T + 643212351220470.36518738495869655884301189962191077376000 * T * T * T * T - 9305948.51220023139345463888692930563770738016256 * B * T * T - 12651111.26276286552615017208089811872955901149184 * M * T * T - 20659.49922948709098011244811411788899593224192 * T * T * TR1 - 836078.14474592016923999505592159092653867139072 * T * T * TR2 + 0.00006789015038990874606020112562595561472 * B * M + 0.00006881971523922700267771186482537037824 * B * TR1 + 0.00006888753756728884459680485448207040512 * B * TR2 + 0.00000000106620689261932842750545416421376 * B * M * TR1 + 0.00000000099838456455748161426067745144832 * B * M * TR2 + 0.00000000006881971523922700533285406113792 * B * TR1 * TR2 - 7.73637926537190144266582551769282215149568 * B * M * T * T - 0.03692776187043992360230216205900391120896 * B * T * T * TR1 - 8.22885124159183316424220536507115805605888 * B * T * T * TR2 - 0.04486152350869073072858690850799967272960 * M * T * T * TR1 - 11.84341809151733719590394656289248191184896 * M * T * T * TR2 - 0.01886579388548595947133545811310221983744 * T * T * TR1 * TR2 + 0.00000000000000099738717738010149484756992 * B * M * TR1 * TR2) * G;
    this->C = -(-0.04521488537456460243017310351196189360128 * B + 353541.92287558489330452100884268484330315055104 * T - 46957916642280.19449418515581184034174215800544424886272 * T * T * T + 428808234146980.24345825663913103922867459974794051584000 * T * T * T * T - 0.00004526010025993916404013408375063707648 * B * M + 52496.95677763570727839941821705680387588489216 * B * T - 0.00004587981015948466845180790988358025216 * B * TR1 - 0.00004592502504485922973120323632138027008 * B * TR2 + 353.89546479846048678298921989721114978287616 * M * T + 358.74106880022581772084263961944634614087680 * T * TR1 + 359.09461072310144209829423123706431410274304 * T * TR2 - 84021060.49315453862908804510125687648984804360192 * B * T * T * T - 102050447.95285830382843366643900735144287759499264 * M * T * T * T - 94328.96942742980039582008701111967406454996992 * T * T * T * TR1 - 42975152.38412545560708231811273025649674924589056 * T * T * T * TR2 + 0.80951464395143729345675278900946114969600 * B * M * T - 0.00000000071080459507955228500363610947584 * B * M * TR1 - 0.00000000066558970970498774284045163429888 * B * M * TR2 + 0.00408583419118973045071351436316734652416 * B * T * TR1 + 0.05587191592001334800161415646237729751040 * B * T * TR2 - 0.00000000004587981015948467022190270742528 * B * TR1 * TR2 + 0.00555788699344118005490580804714631266304 * M * T * TR1 + 0.00520434507056559480433836825700158406656 * M * T * TR2 + 0.00035874106880022586012391576260256464896 * T * TR1 * TR2 + 0.00000000340018355925034627033076473004032 * B * M * T * TR1 + 0.00000075733421809702709126128373394833408 * B * M * T * TR2 - 0.00000000000000066492478492006766323171328 * B * M * TR1 * TR2 + 0.00000000361239359699079247005127633010688 * B * T * TR1 * TR2 + 0.00000000519914592464095387071871576965120 * M * T * TR1 * TR2) * G;
    this->D = -(+0.01130372134364115060754327587799047340032 * B - 176770.96143779244665226050442134242165157527552 * T - 176.94773239923024339149460994860557489143808 * M * T - 179.37053440011290886042131980972317307043840 * T * TR1 - 179.54730536155072104914711561853215705137152 * T * TR2 + 4652974.25610011569672731944346465281885369008128 * B * T * T - 42010530.24657726931454402255062843824492402180096 * B * T * T * T + 6325555.63138143276307508604044905936477950574592 * M * T * T - 51025223.97642915191421683321950367572143879749632 * M * T * T * T + 10329.74961474354549005622405705894449796612096 * T * T * TR1 + 418039.07237296008461999752796079546326933569536 * T * T * TR2 - 47164.48471371490019791004350555983703227498496 * T * T * T * TR1 - 21487576.19206272780354115905636512824837462294528 * T * T * T * TR2 + 407710206613.02374846017593738533909861978651871936512 * T * T - 23478958321140.09724709257790592017087107900272212443136 * T * T * T + 107202058536745.06086456415978275980716864993698512896000 * T * T * T * T + 0.00001131502506498479101003352093765926912 * B * M - 26248.47838881785363919970910852840193794244608 * B * T + 0.00001146995253987116711295197747089506304 * B * TR1 + 0.00001148125626121480743280080908034506752 * B * TR2 - 0.40475732197571864672837639450473057484800 * B * M * T + 0.00000000017770114876988807125090902736896 * B * M * TR1 + 0.00000000016639742742624693571011290857472 * B * M * TR2 - 0.00204291709559486522535675718158367326208 * B * T * TR1 - 0.02793595796000667400080707823118864875520 * B * T * TR2 + 0.00000000001146995253987116755547567685632 * B * TR1 * TR2 - 0.00277894349672059002745290402357315633152 * M * T * TR1 - 0.00260217253528279740216918412850079203328 * M * T * TR2 - 0.00017937053440011293006195788130128232448 * T * TR1 * TR2 + 3.86818963268595072133291275884641107574784 * B * M * T * T + 0.01846388093521996180115108102950195560448 * B * T * T * TR1 + 4.11442562079591658212110268253557902802944 * B * T * T * TR2 + 0.02243076175434536536429345425399983636480 * M * T * T * TR1 + 5.92170904575866859795197328144624095592448 * M * T * T * TR2 + 0.00943289694274297973566772905655110991872 * T * T * TR1 * TR2 - 0.00000000170009177962517313516538236502016 * B * M * T * TR1 - 0.00000037866710904851354563064186697416704 * B * M * T * TR2 + 0.00000000000000016623119623001691580792832 * B * M * TR1 * TR2 - 0.00000000180619679849539623502563816505344 * B * T * TR1 * TR2 - 0.00000000259957296232047693535935788482560 * M * T * TR1 * TR2) * G;

    this->E = (+4027971.58756583523155850680214954774953359048704 * B * T * T + 5916962.45381602729876936696061507665516944162816 * M * T * T + 8842.08490585196215872031818855878091469750272 * T * T * TR2 + 0.00001130372134364115056986896299372642304 * B * M + 176.77096143779245537939396284922675731628032 * B * T + 0.00001130372134364115056986896299372642304 * B * TR2 + 176.77096143779245537939396284922675731628032 * M * T + 176.77096143779245537939396284922675731628032 * T * TR2 + 0.37862932923118854689210246793471159435264 * B * M * T + 0.00000000016623119623001691852489561337845 * B * M * TR1 + 0.00000000016623119623001691852489561337845 * B * M * TR2 + 0.00176841698117039259279305097898597810176 * B * T * TR1 + 0.00176841698117039259279305097898597810176 * B * T * TR2 + 0.00259957296232047714672908623097433686016 * M * T * TR1 + 0.00259957296232047714672908623097433686016 * M * T * TR2) * G;
    this->F = (-0.00004521488537456460227947585197490569216 * B * M - 353.54192287558491075878792569845351463256064 * B * T - 0.00004521488537456460227947585197490569216 * B * TR2 - 353.54192287558491075878792569845351463256064 * M * T - 353.54192287558491075878792569845351463256064 * T * TR2 - 0.75725865846237709378420493586942318870528 * B * M * T - 0.00000000066492478492006767409958245351380 * B * M * TR1 - 0.00000000066492478492006767409958245351380 * B * M * TR2 - 0.00353683396234078518558610195797195620352 * B * T * TR1 - 0.00353683396234078518558610195797195620352 * B * T * TR2 - 0.00519914592464095429345817246194867372032 * M * T * TR1 - 0.00519914592464095429345817246194867372032 * M * T * TR2) * G;
    this->G = (-8055943.17513167046311701360429909549906718097408 * B * T * T - 11833924.90763205459753873392123015331033888325632 * M * T * T - 17684.16981170392431744063637711756182939500544 * T * T * TR2 + 0.00006782232806184690341921377796235853824 * B * M + 0.00006782232806184690341921377796235853824 * B * TR2 + 0.00000000099738717738010151114937368027070 * B * M * TR1 + 0.00000000099738717738010151114937368027070 * B * M * TR2) * G;
    this->H = (-0.00004521488537456460227947585197490569216 * B * M + 353.54192287558491075878792569845351463256064 * B * T - 0.00004521488537456460227947585197490569216 * B * TR2 + 353.54192287558491075878792569845351463256064 * M * T + 353.54192287558491075878792569845351463256064 * T * TR2 + 0.75725865846237709378420493586942318870528 * B * M * T - 0.00000000066492478492006767409958245351380 * B * M * TR1 - 0.00000000066492478492006767409958245351380 * B * M * TR2 + 0.00353683396234078518558610195797195620352 * B * T * TR1 + 0.00353683396234078518558610195797195620352 * B * T * TR2 + 0.00519914592464095429345817246194867372032 * M * T * TR1 + 0.00519914592464095429345817246194867372032 * M * T * TR2) * G;
    this->I = (+4027971.58756583523155850680214954774953359048704 * B * T * T - 176.77096143779245537939396284922675731628032 * T * TR2 - 176.77096143779245537939396284922675731628032 * M * T + 5916962.45381602729876936696061507665516944162816 * M * T * T + 8842.08490585196215872031818855878091469750272 * T * T * TR2 + 0.00001130372134364115056986896299372642304 * B * M - 176.77096143779245537939396284922675731628032 * B * T + 0.00001130372134364115056986896299372642304 * B * TR2 - 0.37862932923118854689210246793471159435264 * B * M * T + 0.00000000016623119623001691852489561337845 * B * M * TR1 + 0.00000000016623119623001691852489561337845 * B * M * TR2 - 0.00176841698117039259279305097898597810176 * B * T * TR1 - 0.00176841698117039259279305097898597810176 * B * T * TR2 - 0.00259957296232047714672908623097433686016 * M * T * TR1 - 0.00259957296232047714672908623097433686016 * M * T * TR2) * G;

}

void Stage4::initParameters(std::atomic<float>* bassParameter, std::atomic<float>* midParameter, std::atomic<float>* trebleParameter) {
    this->bassParameter = bassParameter;
    this->midParameter = midParameter;
    this->trebleParameter = trebleParameter;
}

void Stage4::resetParameters(double sampleRate){
    /*this->A.reset(sampleRate, 0.05);
    this->B.reset(sampleRate, 0.05);
    this->C.reset(sampleRate, 0.05);
    this->D.reset(sampleRate, 0.05);
    this->E.reset(sampleRate, 0.05);
    this->F.reset(sampleRate, 0.05);
    this->G.reset(sampleRate, 0.05);
    this->H.reset(sampleRate, 0.05);
    this->I.reset(sampleRate, 0.05);*/
}
