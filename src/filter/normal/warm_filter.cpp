//
// Created by zhouhaibo on 2017/9/25.
//

#include "warm_filter.h"

static ConstString Warm_FSH = GLES_SHADER_STRING_PRECISION_H(
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;
        uniform sampler2D inputImageTexture1;
        uniform sampler2D inputImageTexture2;
        uniform sampler2D inputImageTexture3;

        void main()
        {
            lowp vec4 textureColor;
            vec4 greyColor;
            vec4 layerColor;

            float xCoordinate = textureCoordinate.x;
            float yCoordinate = textureCoordinate.y;

            highp float redCurveValue;
            highp float greenCurveValue;
            highp float blueCurveValue;

            textureColor = texture2D( inputImageTexture, vec2(xCoordinate, yCoordinate));

            greyColor = texture2D(inputImageTexture2, vec2(xCoordinate, yCoordinate));
            layerColor = texture2D(inputImageTexture3, vec2(xCoordinate, yCoordinate));

            // step1 curve
            redCurveValue = texture2D(inputImageTexture1, vec2(textureColor.r, 0.0)).r;
            greenCurveValue = texture2D(inputImageTexture1, vec2(textureColor.g, 0.0)).g;
            blueCurveValue = texture2D(inputImageTexture1, vec2(textureColor.b, 0.0)).b;

            // step2 curve with mask
            textureColor = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);

            redCurveValue = texture2D(inputImageTexture1, vec2(textureColor.r, 0.0)).a;
            greenCurveValue = texture2D(inputImageTexture1, vec2(textureColor.g, 0.0)).a;
            blueCurveValue = texture2D(inputImageTexture1, vec2(textureColor.b, 0.0)).a;

            lowp vec4 textureColor2 = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);

            // step3 screen with 60%
            lowp vec4 base = vec4(mix(textureColor.rgb, textureColor2.rgb, 1.0 - greyColor.r), textureColor.a);
            lowp vec4 overlayer = vec4(layerColor.r, layerColor.g, layerColor.b, 1.0);

            // screen blending
            textureColor = 1.0 - ((1.0 - base) * (1.0 - overlayer));
            textureColor = (textureColor - base) * 0.6 + base;

            redCurveValue = texture2D(inputImageTexture1, vec2(textureColor.r, 1.0)).r;
            greenCurveValue = texture2D(inputImageTexture1, vec2(textureColor.g, 1.0)).g;
            blueCurveValue = texture2D(inputImageTexture1, vec2(textureColor.b, 1.0)).b;
            textureColor = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);

            gl_FragColor = vec4(textureColor.r, textureColor.g, textureColor.b, 1.0);
        }
);

WarmFilter::WarmFilter() {

}

WarmFilter::~WarmFilter() {

}

void WarmFilter::init(FilterConfig *filterConfig) {
    mFragmentShader = Warm_FSH;
    MultiInputFilter::init(filterConfig);
}