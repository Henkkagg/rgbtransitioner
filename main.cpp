#include <iostream>
#include <functional>
#include <vector>
#include <deque>
#include <math.h>
#include <thread>
#include <chrono>

using namespace std;

struct RGBColors {
    int red;
    int green;
    int blue;
    
    RGBColors(int red, int green, int blue) : red(red), green(green), blue(blue) {}
    RGBColors() {}
};

enum ChangeType {
    DUSK,
    DAWN
};

class ChangeHandler {
    
    RGBColors userColors;
    RGBColors sunDownColors = RGBColors(253, 94, 83);
    function <void(RGBColors)> onColorChange;
    
    short int currentMinute = 0;
    deque<RGBColors> colorDeque;
    
    int getNormalized(int startValue, int endValue, double logValue, double maxLogValue) {
        
        return startValue + (logValue / maxLogValue) * (endValue - startValue);
    }
    
    void initializeColorQueue() {
        
        int steps = 30;
        double logValues[steps];
        double x = 1.0;
        double steepness = 2.0;
        
        for (int i = 0; i < steps; i++) {
            logValues[i] = x * log(steepness * (static_cast<double>(i) / steps) + 1);
        }
        
        double maxLogValue = x * log(steepness + 1);
        for (int i = 0; i < steps; i++) {
            int newRed = getNormalized(userColors.red, sunDownColors.red, logValues[i], maxLogValue);
            int newGreen = getNormalized(userColors.green, sunDownColors.green, logValues[i], maxLogValue);
            int newBlue = getNormalized(userColors.blue, sunDownColors.blue, logValues[i], maxLogValue);
            
            RGBColors newColor(newRed, newGreen, newBlue);
            colorDeque.push_back(newColor);
        }
        
        //Lisätään viimenen manuaalisesti double->int pyöristysvirheiden vuoksi
        colorDeque.push_back(sunDownColors);
    }
    
    void runListener(ChangeType& changeType) {
        while(!colorDeque.empty()) {
            
            //Auringon nousussa ja laskussa otetaan eri päin
            if (changeType == DUSK) {
                onColorChange(colorDeque.back());
                colorDeque.pop_back();
            } else {
                onColorChange(colorDeque.front());
                colorDeque.pop_front();
            }
            
            this_thread::sleep_for(chrono::minutes(1));
        }
    }
    
    ChangeHandler(RGBColors userColors, ChangeType changeType, function<void(RGBColors)> onColorChange)
    : userColors(userColors), onColorChange(onColorChange) {}
    
public:
    
    static ChangeHandler buildAndListen(RGBColors userColors, ChangeType changeType, function<void(RGBColors)> onColorChange) {
        
        ChangeHandler changeHandler(userColors, changeType, onColorChange);
        
        changeHandler.initializeColorQueue();
        changeHandler.runListener(changeType);
        
        return changeHandler;
    }
};

//Kaikki tästä alaspäin oleva koodi on testausta varten

function rgbOutputter = [](RGBColors rgbColors) {
    cout << "Red: " << rgbColors.red << ". ";
    cout << "Green: " << rgbColors.green << ". ";
    cout << "Blue: " << rgbColors.blue << ". ";
    cout << endl;
};

int main() {
    
    RGBColors currentColors(42, 72, 219);
    ChangeType changeType(DUSK);
    
    ChangeHandler::buildAndListen(currentColors, changeType, rgbOutputter);
    
    return 0;
}

