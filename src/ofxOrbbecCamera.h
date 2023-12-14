#pragma once
#include "ofMain.h"

#include "libobsensor/ObSensor.hpp"
#include "libobsensor/hpp/Error.hpp"
#include <opencv2/opencv.hpp>


//If you have ffmpeg / libavcodec included in your project uncomment below 
//You can easily get the required libs from ofxFFmpegRTSP addon ( if you add it to your project )
//#define OFXORBBEC_DECODE_H264_H265

// this allows us to decode the color video streams from Femto Mega over IP connection 
#ifdef OFXORBBEC_DECODE_H264_H265
    extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    }
#endif 

namespace ofxOrbbec{

struct Settings{

    struct FrameType{
        int requestWidth = 0; //needed
        int requestHeight = 0;//usually not needed, width is enough 
        OBFormat format = OB_FORMAT_UNKNOWN; //DEPTH: OB_FORMAT_Y16 COLOR: OB_FORMAT_RGB or OB_FORMAT_MJPG 
        int frameRate = 30;
    };

    std::string ip = "";
    int deviceID = 0;
    std::string deviceSerial = "";

    FrameType depthFrameSize;
    FrameType colorFrameSize; 
    
    bool bColor = false;
    bool bDepth = false; 
    bool bPointCloud = false; 
    bool bPointCloudRGB = false; 
};

};


class ofxOrbbecCamera : public ofThread {
    public:

        ofxOrbbecCamera() = default; 
        ofxOrbbecCamera( const ofxOrbbecCamera & A) = default; 
        ~ofxOrbbecCamera();

        bool open(ofxOrbbec::Settings aSettings);
        bool isConnected();
        void close();
        void update(ofEventArgs& args);
        void loop();
        void threadedFunction();

        static std::vector < std::shared_ptr<ob::DeviceInfo> > getDeviceList(); 

        //any frame
        bool isFrameNew();
        bool isFrameNewDepth();
        bool isFrameNewColor();
        bool isFrameNewIR();

        ofPixels getDepthPixels();
        ofFloatPixels getDepthPixelsF(); 
        ofPixels getColorPixels(); 
        
        std::vector <glm::vec3> getPointCloud(); 
        ofMesh getPointCloudMesh();

    protected:
        void clear();
        
        static std::shared_ptr<ob::Context> & getContext(); 
		static std::shared_ptr <ob::Context> ctx;

        ofPixels processFrame(shared_ptr<ob::Frame> frame);
        void pointCloudToMesh(shared_ptr<ob::Frame> frame, bool bRGB = false);
        
        ofxOrbbec::Settings mCurrentSettings;
        bool bNewFrameColor, bNewFrameDepth, bNewFrameIR = false; 

        ofPixels mDepthPixels, mColorPixels; 
        ofFloatPixels mDepthPixelsF;

        ofMesh mPointCloudMesh; 
        vector <glm::vec3> mPointCloudPts;

		std::shared_ptr <ob::Pipeline> mPipe;
   		std::shared_ptr <ob::PointCloudFilter> pointCloud;
    
        mutable ofMutex colorPixelsMutex, depthPixelsMutex, pointCloudMutex;

        #ifdef OFXORBBEC_DECODE_H264_H265 

            bool bInitOneTime = false; 

            AVCodec* codec264 = nullptr;
            AVCodecContext* codecContext264 = nullptr;

            AVCodec* codec265 = nullptr;
            AVCodecContext* codecContext265 = nullptr;

            SwsContext* swsContext = nullptr;

            void initH26XCodecs();
            ofPixels decodeH26XFrame(uint8_t * myData, int dataSize, bool bH264);

        #endif 

};
