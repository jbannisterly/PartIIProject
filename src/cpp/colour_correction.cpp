#include "colour_correction.hpp"

namespace ColourCorrection {
    std::vector<uint8_t> MethodAverage(std::vector<uint8_t> &data, BarcodeLayout layout, bool allPixels) {
        double count = 0;
        double red = 0;
        double green = 0;
        double blue = 0;
        int barcodeHeight = layout.mask.size() / layout.barcodeWidth;
        
        for (int i = 0; i < layout.mask.size(); i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;
            if ((layout.mask[i] == 0 && ix > 0 && ix < layout.barcodeWidth - 1 && iy > 0 && iy < barcodeHeight - 1) || allPixels) {
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

        double averageColour;
        if (allPixels) {
            averageColour = 127.;
        } else {
            averageColour = 255.;
        }

        for (int i = 0; i < data.size() / 3; i++) {
            double newData = (averageColour / red * data[i * 3 + 0]);
            if (newData > 255) newData = 255;
            data[i * 3 + 0] = newData;
            
            newData = (averageColour / green * data[i * 3 + 1]);
            if (newData > 255) newData = 255;
            data[i * 3 + 1] = newData;
            
            newData = (averageColour / blue * data[i * 3 + 2]);
            if (newData > 255) newData = 255;
            data[i * 3 + 2] = newData;
        }

        return data;
    }

    std::vector<uint8_t> MethodHighLowAvg(std::vector<uint8_t> &data, BarcodeLayout layout) {
        double countH = 0;
        double countL = 0;
        double redH = 0;
        double greenH = 0;
        double blueH = 0;
        double redL = 0;
        double blueL = 0;
        double greenL = 0;
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

    std::vector<uint8_t> MethodQuarters(std::vector<uint8_t> &data, BarcodeLayout layout) {
        std::array<double, 4> countH;
        std::array<double, 4> countL;
        std::array<double, 4> redH;
        std::array<double, 4> greenH;
        std::array<double, 4> blueH;
        std::array<double, 4> redL;
        std::array<double, 4> blueL;
        std::array<double, 4> greenL;
        int barcodeHeight = layout.mask.size() / layout.barcodeWidth;

        for (int i = 0; i < 4; i++) {
            countH[i] = 0;
            countL[i] = 0;
            redH[i] = 0;
            redL[i] = 0;
            greenH[i] = 0;
            greenL[i] = 0;
            blueH[i] = 0;
            blueL[i] = 0;
        }
        
        for (int i = 0; i < layout.mask.size(); i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;
            
            int index = 0;
            if (ix > layout.barcodeWidth / 2) index += 1;
            if (iy > barcodeHeight / 2) index += 2;

            if ((layout.mask[i] == 0 && ix > 0 && ix < layout.barcodeWidth - 1 && iy > 0 && iy < barcodeHeight - 1)) {
                if (layout.data[i * 3] == 255) {
                    countH[index]++;
                    redH[index] += data[i * 3 + 0]; 
                    greenH[index] += data[i * 3 + 1]; 
                    blueH[index] += data[i * 3 + 2]; 
                } else {
                    countL[index]++;
                    redL[index] += data[i * 3 + 0]; 
                    greenL[index] += data[i * 3 + 1]; 
                    blueL[index] += data[i * 3 + 2]; 
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            redH[i] /= countH[i];
            greenH[i] /= countH[i];
            blueH[i] /= countH[i];
            redL[i] /= countL[i];
            greenL[i] /= countL[i];
            blueL[i] /= countL[i];
        }


        for (int i = 0; i < data.size() / 3; i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;

            int index = 0;
            if (ix > layout.barcodeWidth / 2) index += 1;
            if (iy > barcodeHeight / 2) index += 2;
 

            double newData = (255. / (redH[index] - redL[index]) * (double(data[i * 3 + 0]) - redL[index]));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 0] = newData;
            
            newData = (255. / (greenH[index] - greenL[index]) * (double(data[i * 3 + 1]) - greenL[index]));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 1] = newData;
            
            newData = (255. / (blueH[index] - blueL[index]) * (double(data[i * 3 + 2]) - blueL[index]));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 2] = newData;
        }

        return data;
    }

    std::vector<uint8_t> MethodQuartersAverage(std::vector<uint8_t> &data, BarcodeLayout layout, bool allPixels) {
        std::array<double, 4> count;
        std::array<double, 4> red;
        std::array<double, 4> green;
        std::array<double, 4> blue;
        int barcodeHeight = layout.mask.size() / layout.barcodeWidth;

        for (int i = 0; i < 4; i++) {
            count[i] = 0;
            red[i] = 0;
            green[i] = 0;
            blue[i] = 0;
        }
        
        for (int i = 0; i < layout.mask.size(); i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;
            
            int index = 0;
            if (ix > layout.barcodeWidth / 2) index += 1;
            if (iy > barcodeHeight / 2) index += 2;

            if ((layout.mask[i] == 0 && ix > 0 && ix < layout.barcodeWidth - 1 && iy > 0 && iy < barcodeHeight - 1) || allPixels) {
                count[index]++;
                red[index] += data[i * 3 + 0]; 
                green[index] += data[i * 3 + 1]; 
                blue[index] += data[i * 3 + 2]; 
            }
        }

        for (int i = 0; i < 4; i++) {
            red[i] /= count[i];
            green[i] /= count[i];
            blue[i] /= count[i];
        }


        for (int i = 0; i < data.size() / 3; i++) {
            int ix = i % layout.barcodeWidth;
            int iy = i / layout.barcodeWidth;

            int index = 0;
            if (ix > layout.barcodeWidth / 2) index += 1;
            if (iy > barcodeHeight / 2) index += 2;
 

            double newData = (255. / (red[index]) * (double(data[i * 3 + 0])));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 0] = newData;
            
            newData = (255. / (green[index]) * (double(data[i * 3 + 1])));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 1] = newData;
            
            newData = (255. / (blue[index]) * (double(data[i * 3 + 2])));
            if (newData > 255) newData = 255;
            if (newData < 0) newData = 0;
            data[i * 3 + 2] = newData;
        }

        return data;
    }


}