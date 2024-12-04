#include "Stage3.h"

static std::array<float, 4> matrixMul(const std::array<float, 16>& A, const std::array<float, 4>& B) {
    std::array<float, 4> C;
    C[0] = A[0] * B[0] + A[1] * B[1] + A[2] * B[2] + A[3] * B[3];
    C[1] = A[4] * B[0] + A[5] * B[1] + A[6] * B[2] + A[7] * B[3];
    C[2] = A[8] * B[0] + A[9] * B[1] + A[10] * B[2] + A[11] * B[3];
    C[3] = A[12] * B[0] + A[13] * B[1] + A[14] * B[2] + A[15] * B[3];
    return C;
}

static std::array<float, 16> matrixMul(const std::array<float, 16>& A, const std::array<float, 16>& B) {
    std::array<float, 16> C;
    C[0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8] + A[3] * B[12];
    C[1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9] + A[3] * B[13];
    C[2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10] + A[3] * B[14];
    C[3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3] * B[15];
    
    C[4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8] + A[7] * B[12];
    C[5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9] + A[7] * B[13];
    C[6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10] + A[7] * B[14];
    C[7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7] * B[15];

    C[8] = A[8] * B[0] + A[9] * B[4] + A[10] * B[8] + A[11] * B[12];
    C[9] = A[8] * B[1] + A[9] * B[5] + A[10] * B[9] + A[11] * B[13];
    C[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10] + A[11] * B[14];
    C[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11] * B[15];

    C[12] = A[12] * B[0] + A[13] * B[4] + A[14] * B[8] + A[15] * B[12];
    C[13] = A[12] * B[1] + A[13] * B[5] + A[14] * B[9] + A[15] * B[13];
    C[14] = A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14];
    C[15] = A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15];
    return C;
}

static float matrixMul(const std::array<float, 4>& A, const std::array<float, 4>& B) {
    return A[0] * B[0] + A[1] * B[1] + A[2] * B[2] + A[3] * B[3];
}

static std::array<float, 4> matrixMul(const std::array<float, 4>& A, float B) {
    std::array<float, 4> C{ 0.0f };
    for (int i = 0; i < A.size(); i++) {
        C[i] = B * A[i];
    }
    return C;
}

static std::array<float, 4> matrixAdd(const std::array<float, 4>& A, const std::array<float, 4>& B) {
    std::array<float, 4> C;
    C[0] = A[0] + B[0];
    C[1] = A[1] + B[1];
    C[2] = A[2] + B[2];
    C[3] = A[3] + B[3];
    return C;
}

static bool gluInvertMatrix(const std::array<float, 16>& m, std::array<float, 16>& invOut)
{
    std::array<float, 16> inv;
    float det;
    int i;

    inv[0] = m[5] * m[10] * m[15] -
        m[5] * m[11] * m[14] -
        m[9] * m[6] * m[15] +
        m[9] * m[7] * m[14] +
        m[13] * m[6] * m[11] -
        m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] +
        m[4] * m[11] * m[14] +
        m[8] * m[6] * m[15] -
        m[8] * m[7] * m[14] -
        m[12] * m[6] * m[11] +
        m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] -
        m[4] * m[11] * m[13] -
        m[8] * m[5] * m[15] +
        m[8] * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] +
        m[4] * m[10] * m[13] +
        m[8] * m[5] * m[14] -
        m[8] * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] +
        m[1] * m[11] * m[14] +
        m[9] * m[2] * m[15] -
        m[9] * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
        m[0] * m[11] * m[14] -
        m[8] * m[2] * m[15] +
        m[8] * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] +
        m[0] * m[11] * m[13] +
        m[8] * m[1] * m[15] -
        m[8] * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
        m[0] * m[10] * m[13] -
        m[8] * m[1] * m[14] +
        m[8] * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] -
        m[1] * m[7] * m[14] -
        m[5] * m[2] * m[15] +
        m[5] * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
        m[0] * m[7] * m[14] +
        m[4] * m[2] * m[15] -
        m[4] * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
        m[0] * m[7] * m[13] -
        m[4] * m[1] * m[15] +
        m[4] * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] +
        m[0] * m[6] * m[13] +
        m[4] * m[1] * m[14] -
        m[4] * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

static float closestElementLinearInterpolate(const float x, const std::vector<float>& xArray, const std::vector<float>& yArray) {
    size_t size = xArray.size();
    if (x < xArray[0]) {
        return yArray[0];
    }
    else if (x > xArray[size - 1]) {
        return yArray[size - 1];
    }
    int i = 0, j = static_cast<int>(size), mid = 0;
    while (i < j) {
        mid = (i + j) / 2;

        if (xArray[mid] == x) {
            return yArray[mid];
        }

        if (x < xArray[mid]) {
            if (mid > 0 && x > xArray[mid - 1]) {
                float div = xArray[mid] - xArray[mid - 1];
                float mull = (xArray[mid] - x) / div;
                float mulr = (x - xArray[mid - 1]) / div;
                return yArray[mid - 1] * mull + yArray[mid] * mulr;
            }
            j = mid;
        }
        else {
            if (mid < size - 1 && x < xArray[mid + 1]) {
                float div = xArray[mid + 1] - xArray[mid];
                float mull = (xArray[mid + 1] - x) / div;
                float mulr = (x - xArray[mid]) / div;
                return yArray[mid] * mull + yArray[mid + 1] * mulr;
            }
            i = mid + 1;
        }
    }
    return yArray[mid];
}

void Stage3::configure(double sampleRate) {
    this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
    float T = 1 / this->sampleRate;
    float h = static_cast<float>(2 * this->sampleRate);

    const double R22 = 47000;
    const float R33 = 6800;
    const float R34 = 1000;
    float R37; //PARAMETER
    if (*this->odButton > 0.5) {
        R37 = 50000.0f + 0.01f - *this->odParameter;
    }
    else {
        R37 = 100000;
    }
    const float C12 = 220e-9f;
    const float C18 = 470e-12f;
    const float E13 = 2.2e-6f;
    const float E14 = 2.2e-6f;

    std::array<float, 16> A{ 0.0f };
    std::array<float, 4> B{ 0.0f };
    std::array<float, 4> C{ 0.0f };


    A[0] = -1 / (C12 * (R37 + R34)) - 1 / (C12 * R33);
    A[2] = -1 / (C12 * (R37 + R34));
    A[4] = 1 / (C18 * (R37 + R34));
    A[5] = -1 / (R22 * C18);
    A[6] = 1 / (C18 * (R37 + R34));
    A[8] = -1 / (E13 * (R37 + R34));
    A[10] = -1 / (E13 * (R37 + R34));

    B[0] = -1 / (C12 * (R37 + R34));
    B[1] = 1 / (C18 * (R37 + R34));
    B[2] = -1 / (E13 * (R37 + R34));

    C[0] = R37 / (C12 * (R37 + R34));
    C[1] = -R37 / (C18 * (R37 + R34));
    C[2] = R37 / (E13 * (R37 + R34));
    C[3] = -1 / E14;

    this->D[0] = -R37 / (R37 + R34);
    this->D[1] = 1;
    this->D[2] = -R37 / (R37 + R34);
    this->D[3] = 1;

    this->E = R34 / (R34 + R37);
    const float F = -R34 * R37 / (R37 + R34);

    const std::array<float, 16> I { h,0,0,0,0,h,0,0,0,0,h,0,0,0,0,h };
    std::array<float, 16> IhmA{0};
    std::array<float, 16> IhpA{0};
    for (int i = 0; i < I.size(); i++) {
        IhmA[i] = I[i] - A[i];
        IhpA[i] = I[i] + A[i];
    }
    std::array<float, 16> inv{0};
    gluInvertMatrix(IhmA, inv);

    this->G = matrixMul(inv, C);
    this->H = matrixMul(inv, IhpA);
    this->J = matrixMul(inv, B);
    this->K = matrixMul(this->D, this->G) + F;

    for (int i = 0; i < this->x.size(); i++) {
        this->p[i] = (this->x[i] - this->K * this->y[i]);
    }


    // saturation parameters
    A[1] = 1 / (C12 * (R37 + R34));
    A[5] = -1 / (R22 * C18) - 1 / (C18 * (R37 + R34));
    A[9] = 1 / (E13 * (R34 + R37));
    
    this->DSat[0] = -R37 / (R37 + R34);
    this->DSat[1] = R37 / (R37 + R34);
    this->DSat[2] = -R37 / (R37 + R34);
    this->DSat[3] = 1;

    for (int i = 0; i < I.size(); i++) {
        IhmA[i] = I[i] - A[i];
        IhpA[i] = I[i] + A[i];
    }
    gluInvertMatrix(IhmA, inv);

    this->GSat = matrixMul(inv, C);
    this->HSat = matrixMul(inv, IhpA);
    this->JSat = matrixMul(inv, B);
    this->KSat = matrixMul(this->DSat, this->GSat) + F;

    for (int i = 0; i < this->x.size(); i++) {
        this->pSat[i] = (this->x[i] - this->KSat * this->y[i]);
    }
}

void Stage3::initParameters(std::atomic<float>* odParameter, std::atomic<float>* odButton) {
    this->odParameter = odParameter;
    this->odButton = odButton;
    
    juce::StringArray hardclip(juce::StringArray::fromLines(BinaryData::hardclip_txt));
    for (juce::String i : hardclip) {
        float x = i.substring(0, i.indexOfChar('\t')).getFloatValue();
        float y = i.substring(i.indexOfChar('\t') + 1, i.length()).getFloatValue();
        this->x.push_back(x);
        this->y.push_back(y);
    }
    this->p.resize(this->x.size());
    this->pSat.resize(this->x.size());
}
Stage3::Stage3():Stage(true), R35(2700.0f), cutOffVoltage(TL072_CUTOFF){}

void Stage3::processBlock(juce::AudioBuffer<float>& buffer) {

    int channelNumber = buffer.getNumChannels();
    this->w.resize(channelNumber, std::array<float, 4>{0.0f});
    this->yBuffer.resize(channelNumber, 0.0f);
    this->uBuffer.resize(channelNumber, 0.0f);

    std::array<float, 4> pk{ 0.0f };
    float p;

    for (int channel = 0; channel < channelNumber; channel++) {
        auto channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {

            float tmp = channelSamples[i];
            std::array<float, 4> Hw = matrixMul(H, w[channel]);
            std::array<float, 4> Ju = matrixMul(J, channelSamples[i] + uBuffer[channel]);
            std::array<float, 4> Gy = matrixMul(G, yBuffer[channel]);

            pk = matrixAdd(matrixAdd(Hw, Ju), Gy);

            p = matrixMul(D, pk) + E * channelSamples[i];
            ySave = yBuffer[channel];
            this->yBuffer[channel] = closestElementLinearInterpolate(p, this->p, this->y);

            Gy = matrixMul(G, yBuffer[channel]);
            wSave = w[channel];
            w[channel] = matrixAdd(Gy, pk);
            
            //saturation
            if (channelSamples[i] + w[channel][1] > this->cutOffVoltage || channelSamples[i] + w[channel][1] < -this->cutOffVoltage) {
                float cutOffVoltage;
                if (channelSamples[i] + w[channel][1] > this->cutOffVoltage){
                    cutOffVoltage = this->cutOffVoltage;
                }
                else {
                    cutOffVoltage = -this->cutOffVoltage;
                }
                w[channel] = wSave;
                yBuffer[channel] = ySave;
                Hw = matrixMul(HSat, w[channel]);
                Ju = matrixMul(JSat, cutOffVoltage + uBuffer[channel] + w[channel][1]);
                Gy = matrixMul(GSat, yBuffer[channel]);

                pk = matrixAdd(matrixAdd(Hw, Ju), Gy);

                p = matrixMul(DSat, pk) + E * cutOffVoltage;
                this->yBuffer[channel] = closestElementLinearInterpolate(p, this->pSat, this->y);

                Gy = matrixMul(GSat, yBuffer[channel]);
                w[channel] = matrixAdd(Gy, pk);

                this->uBuffer[channel] = cutOffVoltage-w[channel][1];
                channelSamples[i] = cutOffVoltage + w[channel][3] - R35 * yBuffer[channel];
        


            }
            else {
                this->uBuffer[channel] = channelSamples[i];
                channelSamples[i] = w[channel][1] + w[channel][3] + channelSamples[i] - R35 * yBuffer[channel];
            }


        }
    }

}
