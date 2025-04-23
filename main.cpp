#include <papi.h>

#include <iostream>

#include "CircularBuffer.hpp"
#include "ParameterQueue.hpp"
#include "ReSample.hpp"
#include "Stage.h"
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"

#define t             1
#define fs            48000
#define f0            440
#define channelNumber 2

void generateInput(std::vector<std::vector<float>>& buffer);
void processBlock(std::vector<std::vector<float>>& buffer);
int init_papi();
int create_event_set(int& event_set);
int start_counting(int event_set);
long long stop_counting(int event_set);
void cleanup_papi(int event_set);
void perform_operations();

int main(int argc, char** argv) {
    std::vector<std::vector<float>> buffer;
    generateInput(buffer);
    processBlock(buffer);
    return 0;
}

void generateInput(std::vector<std::vector<float>>& buffer) {
    std::vector<float> channel(t * fs, 0.0f);
    for(int i = 0; i < t * fs; i++) {
        channel[i] = std::sin(2 * 3.141592 * f0 * i / fs);
    }
    buffer.resize(2, channel);
}

void processBlock(std::vector<std::vector<float>>& buffer) {
    // PAPI
    int event_set;
    if(init_papi() != 0) exit;
    if(create_event_set(event_set) != 0) exit;

    Stage1 stage1;
    Stage2 stage2;
    Stage3 stage3;
    Stage4 stage4;
    Stage5 stage5;
    Stage6 stage6;

    ReSample<8, 100> resample;

    float inputGain = 1.5f;

    stage1.configure(fs);
    stage2.configure(fs);
    stage3.configure(fs);
    stage4.configure(fs);
    stage5.configure(fs);
    stage6.configure(fs);

    stage3.initParameters(nullptr, nullptr);

    // Start counting
    if(start_counting(event_set) != 0) exit;

    resample.configure(fs);

    //    buffer.applyGain(inputGain);
    for(int i = 0; i < buffer.size(); i++) {
        for(int j = 0; j < buffer[i].size(); j++) {
            buffer[i][j] *= inputGain;
        }
    }

    stage1.processBlock(buffer);

    resample.interpolate(buffer);

    resample.process([&stage2](std::vector<std::vector<float>>& buffer) {
        stage2.processBlock(buffer);
    });
    resample.process([&stage3](std::vector<std::vector<float>>& buffer) {
        stage3.processBlock(buffer);
    });
    resample.process([&stage4](std::vector<std::vector<float>>& buffer) {
        stage4.processBlock(buffer);
    });
    resample.process([&stage5](std::vector<std::vector<float>>& buffer) {
        stage5.processBlock(buffer);
    });
    resample.process([&stage6](std::vector<std::vector<float>>& buffer) {
        stage6.processBlock(buffer);
    });

    resample.decimate(buffer);

    float outputGain = 1.5f;

    // buffer.applyGain(outputGain);
    for(int i = 0; i < buffer.size(); i++) {
        for(int j = 0; j < buffer[i].size(); j++) {
            buffer[i][j] *= outputGain;
        }
    }

    // Stop counting and retrieve the result
    long long ops = stop_counting(event_set);
    if(ops == -1) exit;
    // Output the number of double-precision operations
    std::cout << "Double-precision floating point operations: " << ops
              << std::endl;
    // Clean up PAPI resources
    cleanup_papi(event_set);
}

// Function to initialize the PAPI library
int init_papi() {
    if(PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        std::cerr << "PAPI library init error!" << std::endl;
        return -1;
    }
    return 0;
}

// Function to create a PAPI event set and add the PAPI_DP_OPS event
int create_event_set(int& event_set) {
    event_set = PAPI_NULL;
    if(PAPI_create_eventset(&event_set) != PAPI_OK) {
        std::cerr << "Failed to create event set" << std::endl;
        return -1;
    }

    if(PAPI_add_event(event_set, PAPI_DP_OPS) != PAPI_OK) {
        std::cerr << "Could not add PAPI_DP_OPS event" << std::endl;
        return -1;
    }

    return 0;
}

// Function to start counting events
int start_counting(int event_set) {
    if(PAPI_start(event_set) != PAPI_OK) {
        std::cerr << "PAPI_start failed" << std::endl;
        return -1;
    }
    return 0;
}

// Function to stop counting and get the result
long long stop_counting(int event_set) {
    long long values[1] = {0};
    if(PAPI_stop(event_set, values) != PAPI_OK) {
        std::cerr << "PAPI_stop failed" << std::endl;
        return -1;
    }
    return values[0];
}

// Function to clean up and remove the event set
void cleanup_papi(int event_set) {
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
}

// Function to perform some double-precision operations
void perform_operations() {
    double result = 0;
    for(int i = 0; i < 1000000; ++i) {
        result +=
            1.0 / (i + 1.0);    // Just some dummy double-precision operations
    }
}