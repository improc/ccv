#pragma once

#include <gtest/gtest.h>
#include "models/mouth_status_model_64x64c3_5_conv.h"

#include <string>
#include "common_operations.h"
extern "C" {
#   include "ccv.h"
}


const std::string modelFilenameSQL("models/mouth_status_model_64x64c3_5");
const std::string modelFilenameText("models/mouth_status_model_64x64c3_5_conv.txt");
const std::string modelFilenameSQL1Ch("models/mouth_status_model_64x64c1_29");

const std::string mouthFilename("images/mouth.bmp");
const std::string mouthFilenameBigger("images/mouth_bigger.bmp");

TEST(ccv_model, modelLoadingFromSQL) 
{
    ccv_convnet_t* model = ccv_convnet_read(0, modelFilenameSQL.c_str());

    EXPECT_TRUE(model != NULL) << "model not loaded correctly";
    EXPECT_EQ(3, model->channels) << "model not loaded correctly - number of channels do not correspond";
    EXPECT_EQ(64, model->rows) << "model not loaded correctly - x size do not correspond";
    EXPECT_EQ(64, model->cols) << "model not loaded correctly - y size do not correspond";

    ccv_convnet_free(model);
    model = NULL;
}

TEST(ccv_model, modelLoadingFromText) 
{
    ccv_convnet_t* model = ccv_convnet_read_file(0, modelFilenameText.c_str());

    EXPECT_TRUE(model != NULL) << "model not loaded correctly";
    EXPECT_EQ(3, model->channels) << "model not loaded correctly - number of channels do not correspond";
    EXPECT_EQ(64, model->rows) << "model not loaded correctly - x size do not correspond";
    EXPECT_EQ(64, model->cols) << "model not loaded correctly - y size do not correspond";

    ccv_convnet_free(model);
    model = NULL;
}


TEST(ccv_model, modelVerificationHeader) 
{
    const std::string tempFileName = "tmp.txt";

    ccv_convnet_t* model = ccv_convnet_read_file(0, modelFilenameText.c_str());
    ccv_convnet_write_param_t params;
    params.half_precision = true;
    ccv_convnet_write_file(model, tempFileName.c_str(), params);

    EXPECT_TRUE(fileComparison(tempFileName, modelFilenameText)) << "model not loaded correctly";

    ccv_convnet_free(model);
    model = NULL;

    // delete tmp
    remove(tempFileName.c_str());
}

TEST(ccv_model, modelVerificationClassification) 
{
    ccv_enable_default_cache();

    ccv_dense_matrix_t* image = NULL;

    ccv_convnet_t* model = ccv_convnet_read_file(0, modelFilenameText.c_str());
    ccv_read_impl(mouthFilename.c_str(),&image, CCV_IO_BMP_FILE,0,0,0);
 
    EXPECT_TRUE(image != NULL);
     
    ccv_array_t* rank;
    ccv_convnet_classify(model, &image, 1, &rank, 2, 1);

    ccv_classification_t* classification1 = (ccv_classification_t*)ccv_array_get(rank, 0);
    ccv_classification_t* classification2 = (ccv_classification_t*)ccv_array_get(rank, 1);

    const double value1 = (classification1->id == 0)?classification2->confidence - classification1->confidence: classification1->confidence - classification2->confidence;

    ccv_convnet_free(model);

    model = ccv_convnet_read(0, modelFilenameSQL.c_str());

    ccv_convnet_classify(model, &image, 1, &rank, 2, 1);

    classification1 = (ccv_classification_t*)ccv_array_get(rank, 0);
    classification2 = (ccv_classification_t*)ccv_array_get(rank, 1);

    const double value2 = (classification1->id == 0)?classification2->confidence - classification1->confidence: classification1->confidence - classification2->confidence;
    
    EXPECT_EQ(value1, value2) << "predictions differs";
    
    ccv_convnet_free(model);
    ccv_matrix_free(image);
}

TEST(ccv_model, deepChecking) 
{
    ccv_convnet_t* mod1 = ccv_convnet_read_file(0, modelFilenameText.c_str());
    ccv_convnet_t* mod2 = ccv_convnet_read(0, modelFilenameSQL.c_str());

    EXPECT_EQ(mod1->input.height, mod2->input.height) << "model input.height not equal";
    EXPECT_EQ(mod1->input.width, mod2->input.width) << "model input.width not equal";

    EXPECT_EQ(mod1->rows, mod2->rows) << "model rows not equal";
    EXPECT_EQ(mod1->cols, mod2->cols) << "model cols not equal";
    EXPECT_EQ(mod1->channels, mod2->channels) << "model channels not equal";
    
    EXPECT_EQ(mod1->count, mod2->count) << "model count not equal";
    
    EXPECT_EQ(mod1->mean_activity->cols, mod2->mean_activity->cols) << "model mean_activity->cols not equal";
    EXPECT_EQ(mod1->mean_activity->rows, mod2->mean_activity->rows) << "model mean_activity->rows not equal";

    for (int i = 0; i < mod1->mean_activity->cols * mod1->mean_activity->rows; i++)
    {
        EXPECT_EQ(mod1->mean_activity->data.f32[i], mod2->mean_activity->data.f32[i]) << "model mean_activity->data.f32[i] not equal";
    }

    for (int layerId = 0; layerId < mod1->count; layerId++)
    {
        ccv_convnet_layer_t* layer1 = mod1->layers + layerId;
        ccv_convnet_layer_t* layer2 = mod2->layers + layerId;

        EXPECT_EQ(layer1->type, layer2->type) << "model layer type not equal";
        EXPECT_EQ(layer1->wnum, layer2->wnum) << "model layer wnum not equal";

        if (layer1->type == CCV_CONVNET_CONVOLUTIONAL || layer1->type == CCV_CONVNET_FULL_CONNECT)
        {
            for (unsigned int i = 0U; i < layer1->wnum; i++)
            {
                EXPECT_EQ(layer1->w[i], layer2->w[i]) << "model layer1->w[i] not equal";
            }

            const unsigned int cntBias = (layer1->type == CCV_CONVNET_CONVOLUTIONAL ? layer1->net.convolutional.count : layer1->net.full_connect.count);

            for (unsigned int i = 0U; i < cntBias; i++)
            {
                EXPECT_EQ(layer1->bias[i], layer2->bias[i]) << "model layer1->bias[i] not equal";
            }
        }
    }

    ccv_convnet_free(mod1);
    mod1= NULL;

    ccv_convnet_free(mod2);
    mod2 = NULL;
}