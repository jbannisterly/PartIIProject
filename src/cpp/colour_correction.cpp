#include "colour_correction.hpp"

namespace ColourCorrection {
    std::vector<uint8_t> MethodAverage(std::vector<uint8_t> &data, BarcodeLayout layout) {
        double count;
        double red;
        double green;
        double blue;
        int barcodeHeight = layout.mask.size() / layout.barcodeWidth;
        
        for (int i = 0; i < layout.mask.size(); i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;
            if (layout.mask[i] == 0 && ix > 0 && ix < layout.barcodeWidth - 1 && iy > 0 && iy < barcodeHeight - 1) {
                if (layout.data[i * 3] == 255) {
                    count++;
                    red += data[i * 3 + 0]; 
                    green += data[i * 3 + 1]; 
                    blue += data[i * 3 + 2]; 
                }
            }
        }

        red /= count;
        green /= count;
        blue /= count;

        for (int i = 0; i < data.size() / 3; i++) {
            double newData = (255. / red * data[i * 3 + 0]);
            if (newData > 255) newData = 255;
            data[i * 3 + 0] = newData;
            
            newData = (255. / green * data[i * 3 + 1]);
            if (newData > 255) newData = 255;
            data[i * 3 + 1] = newData;
            
            newData = (255. / blue * data[i * 3 + 2]);
            if (newData > 255) newData = 255;
            data[i * 3 + 2] = newData;
        }

        return data;
    }

    std::vector<uint8_t> MethodHighLowAvg(std::vector<uint8_t> &data, BarcodeLayout layout) {
        double countH;
        double countL;
        double redH;
        double greenH;
        double blueH;
        double redL;
        double blueL;
        double greenL;
        int barcodeHeight = layout.mask.size() / layout.barcodeWidth;
        
        for (int i = 0; i < layout.mask.size(); i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;
            if (layout.mask[i] == 0 && ix > 0 && ix < layout.barcodeWidth - 1 && iy > 0 && iy < barcodeHeight - 1) {
                if (layout.data[i * 3] == 255) {
                    countH++;
                    redH += data[i * 3 + 0]; 
                    greenH += data[i * 3 + 1]; 
                    blueH += data[i * 3 + 2]; 
                } else {
                    countL++;
                    redL += data[i * 3 + 0]; 
                    greenL += data[i * 3 + 1]; 
                    blueL += data[i * 3 + 2]; 
                }
            }
        }

        redH /= countH;
        greenH /= countH;
        blueH /= countH;
        redL /= countL;
        greenL /= countL;
        blueL /= countL;


        for (int i = 0; i < data.size() / 3; i++) {
            double newData = (255. / (redH - redL) * (double(data[i * 3 + 0]) - redL));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 0] = newData;
            
            newData = (255. / (greenH - greenL) * (double(data[i * 3 + 1]) - greenL));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 1] = newData;
            
            newData = (255. / (blueH - blueL) * (double(data[i * 3 + 2]) - blueL));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 2] = newData;
        }

        return data;
    }


}