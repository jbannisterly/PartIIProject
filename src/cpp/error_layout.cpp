#include "error_layout.hpp" 

namespace ErrorLayout {
    std::vector<ErrorCorrectionVirtual*> Bit_9() {
        std::vector<ErrorCorrectionVirtual*> errorCorrectors;
        errorCorrectors.push_back(new ErrorCorrection<255, 128>());
        errorCorrectors.push_back(new ErrorCorrection<255, 128>());
        errorCorrectors.push_back(new ErrorCorrection<255, 128>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());
        errorCorrectors.push_back(new ErrorCorrection<255, 200>());


        return errorCorrectors;
    }

    std::vector<ErrorCorrectionVirtual*> Bit_3() {
        std::vector<ErrorCorrectionVirtual*> errorCorrectors;
        errorCorrectors.push_back(new ErrorCorrection<255, 32>());
        errorCorrectors.push_back(new ErrorCorrection<255, 32>());
        errorCorrectors.push_back(new ErrorCorrection<255, 32>());

        return errorCorrectors;
    }

}