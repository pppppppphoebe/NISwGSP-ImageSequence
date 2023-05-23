//
//  NISwGSP_Stitching.cpp
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#include "NISwGSP_Stitching.h"

NISwGSP_Stitching::NISwGSP_Stitching(MultiImages & _multi_images) : MeshOptimization(_multi_images) {
    
}

void NISwGSP_Stitching::setWeightToAlignmentTerm(const double _weight) {
    MeshOptimization::setWeightToAlignmentTerm(_weight);
}

void NISwGSP_Stitching::setWeightToLocalSimilarityTerm(const double _weight) {
    MeshOptimization::setWeightToLocalSimilarityTerm(_weight);
}

void NISwGSP_Stitching::setWeightToGlobalSimilarityTerm(const double _weight_beta,
                                                        const double _weight_gamma,
                                                        const enum GLOBAL_ROTATION_METHODS _global_rotation_method) {
    MeshOptimization::setWeightToGlobalSimilarityTerm(_weight_beta, _weight_gamma, _global_rotation_method);
}

void NISwGSP_Stitching::solve(const BLENDING_METHODS & _blend_method) {

    TimeCalculator timer;
    Mat result;

    timer.start();
    MultiImages & multi_images = getMultiImages();

    vector<Triplet<double> > triplets;
    vector<pair<int, double> > b_vector;
  
    reserveData(triplets, b_vector, DIMENSION_2D);
    triplets.emplace_back(0, 0, STRONG_CONSTRAINT);
    triplets.emplace_back(1, 1, STRONG_CONSTRAINT);
    b_vector.emplace_back(0,    STRONG_CONSTRAINT);
    b_vector.emplace_back(1,    STRONG_CONSTRAINT);
    timer.end("[feature matching] " + multi_images.parameter.file_name);

    timer.start();
   
    prepareAlignmentTerm(triplets);
    prepareSimilarityTerm(triplets, b_vector);
   
    vector<vector<Point2> > original_vertices;

    original_vertices = getImageVerticesBySolving(triplets, b_vector);
    timer.end("[solve optimization] " + multi_images.parameter.file_name);

    timer.start();
    Size2 target_size = normalizeVertices(original_vertices);

    const Parameter& parameter = multi_images.parameter;
    vector<string> img_sequence = parameter.img_sequence;

    multi_images.meshOptimize2(original_vertices, target_size);
    timer.end("[post process] " + multi_images.parameter.file_name);

    multi_images.textureMapping(original_vertices);
 
    for (int i = 0; i < img_sequence.size(); i++) {
        cout << "Current images: " << img_sequence[i] << endl;
        multi_images.updateImages(i);
        timer.start();
        multi_images.writeWarpingImages(original_vertices, target_size);
        timer.end("[texture mapping] " + multi_images.parameter.file_name);
       
    #ifndef NDEBUG
        if (debugMode) {
            Mat result = multi_images.writeResult(original_vertices, target_size,
                img_sequence[i] + "/" + parameter.file_name + "-[NISwGSP]" +
                GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                BLENDING_METHODS_NAME[_blend_method]
                , _blend_method);
            multi_images.writeResultWithMesh(result, original_vertices, "-[NISwGSP]" +
                GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                BLENDING_METHODS_NAME[_blend_method] +
                "[Mesh]", false);
            multi_images.writeResultWithMesh(result, original_vertices, "-[NISwGSP]" +
                GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                BLENDING_METHODS_NAME[_blend_method] +
                "[Border]", true);
            multi_images.writeMiddleLine(result, original_vertices, "-[NISwGSP]" +
                GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                "[middleLine]", true);
        }
           
    #endif
 
    }
}

void NISwGSP_Stitching::writeImage(const Mat & _image, const string _post_name){

    return;
    MultiImages & multi_images = getMultiImages();
    const Parameter & parameter = multi_images.parameter;
    string file_name = parameter.file_name;
    
    imwrite(parameter.result_dir + file_name + "-" +
            "[NISwGSP]" +
            GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
            _post_name +
            ".png", _image);
}