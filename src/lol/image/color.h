//
//  Lol Engine
//
//  Copyright © 2010—2015 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

//
// The Color class
// ---------------
// Provides various color conversion routines.
//

#include <lol/math/vector.h>

namespace lol
{

class Color : public vec4
{
public:
    /*
     * Convert linear RGB to sRGB
     */
    static vec3 LinearRGBTosRGB(vec3 src)
    {
        vec3 ret = 12.92f * src;
        if (src.r > 0.0031308f)
            ret.r = 1.055f * pow(src.r, 1.0f / 2.4f) - 0.055f;
        if (src.g > 0.0031308f)
            ret.g = 1.055f * pow(src.g, 1.0f / 2.4f) - 0.055f;
        if (src.b > 0.0031308f)
            ret.b = 1.055f * pow(src.b, 1.0f / 2.4f) - 0.055f;
        return ret;
    }

    static vec4 LinearRGBTosRGB(vec4 src)
    {
        return vec4(LinearRGBTosRGB(src.rgb), src.a);
    }

    /*
     * Convert sRGB to linear RGB
     */
    static vec3 sRGBToLinearRGB(vec3 src)
    {
        vec3 ret = 1.0f / 12.92f * src;
        if (src.r > 0.04045f)
            ret.r = pow(src.r + 0.055f, 2.4f) / pow(1.055f, 2.4f);
        if (src.g > 0.04045f)
            ret.g = pow(src.g + 0.055f, 2.4f) / pow(1.055f, 2.4f);
        if (src.b > 0.04045f)
            ret.b = pow(src.b + 0.055f, 2.4f) / pow(1.055f, 2.4f);
        return ret;
    }

    static vec4 sRGBToLinearRGB(vec4 src)
    {
        return vec4(sRGBToLinearRGB(src.rgb), src.a);
    }

    /*
     * Convert linear RGB to YUV
     */
    static vec3 RGBToYUV(vec3 src)
    {
        mat3 m(vec3(0.299f, -0.14713f,  0.615f),
               vec3(0.587f, -0.28886f, -0.51499f),
               vec3(0.114f,  0.436f,   -0.10001f));
        vec3 tmp = m * src;
        tmp.r = (tmp.r * 220.f + 16.f) / 255.f;
        tmp.g += 0.5f;
        tmp.b += 0.5f;
        return clamp(tmp, 0.f, 1.f);
    }

    static vec4 RGBToYUV(vec4 src)
    {
        return vec4(RGBToYUV(src.rgb), src.a);
    }

    /*
     * Convert YUV to linear RGB
     */
    static vec3 YUVToRGB(vec3 src)
    {
        src.r = (src.r * 255.f - 16.f) / 220.f;
        src.g -= 0.5f;
        src.b -= 0.5f;
        mat3 m(vec3(1.f,       1.f,      1.f),
               vec3(0.f,      -0.39465f, 2.03211f),
               vec3(1.13983f, -0.58060f, 0.f));
        return clamp(m * src, 0.f, 1.f);
    }

    static vec4 YUVToRGB(vec4 src)
    {
        return vec4(YUVToRGB(src.rgb), src.a);
    }

    /*
     * Convert linear HSV to linear RGB
     */
    static vec3 HSVToRGB(vec3 src)
    {
        vec3 tmp = vec3(-1.f + abs(6.f * src.x - 3.f),
                         2.f - abs(6.f * src.x - 2.f),
                         2.f - abs(6.f * src.x - 4.f));
        return src.z * mix(vec3(1.f), clamp(tmp, 0.f, 1.f), src.y);
    }

    static vec4 HSVToRGB(vec4 src)
    {
        return vec4(HSVToRGB(src.rgb), src.a);
    }

    /*
     * Convert RGB to HSV
     */
    static vec3 RGBToHSV(vec3 src)
    {
        float K = 0.f;

        if (src.g < src.b)
            src = src.rbg, K = -1.f;

        if (src.r < src.g)
            src = src.grb, K = -2.f / 6.f - K;

        float chroma = src.r - min(src.g, src.b);
        /* XXX: we use min() here because numerical stability is not
         * guaranteed with -ffast-math, I’ve seen it fail on i386. */
        return vec3(min(abs(K + (src.g - src.b) / (6.f * chroma)), 1.f),
                    chroma / (src.r + 1e-20f),
                    src.r);
    }

    static vec4 RGBToHSV(vec4 src)
    {
        return vec4(RGBToHSV(src.rgb), src.a);
    }

    /*
     * Convert RGB to HSL
     */
    static vec3 RGBToHSL(vec3 src)
    {
        float K = 0.f;

        if (src.g < src.b)
            src = src.rbg, K = -1.f;

        if (src.r < src.g)
            src = src.grb, K = -2.f / 6.f - K;

        float chroma = src.r - min(src.g, src.b);
        float luma = src.r + min(src.g, src.b);
        /* XXX: we use min() here because numerical stability is not
         * guaranteed with -ffast-math, I’ve seen it fail on i386. */
        float h = min(abs(K + (src.g - src.b) / (6.f * chroma)), 1.f);
        float s = clamp(chroma / (min(luma, 2.f - luma)), 0.f, 1.f);
        return vec3(h, s, 0.5f * luma);
    }

    static vec4 RGBToHSL(vec4 src)
    {
        return vec4(RGBToHSL(src.rgb), src.a);
    }

    /*
     * Convert linear HSV to linear HSL
     */
    static vec3 HSVToHSL(vec3 src)
    {
        float tmp = (2 - src.y) * src.z;
        return vec3(src.x,
                    src.y * src.z / (min(tmp, 2.f - tmp) + 1e-20f),
                    0.5f * tmp);
    }

    static vec4 HSVToHSL(vec4 src)
    {
        return vec4(HSVToHSL(src.rgb), src.a);
    }

    /*
     * Convert linear HSL to linear HSV
     */
    static vec3 HSLToHSV(vec3 src)
    {
        float tmp = src.y * (0.5f - abs(0.5f - src.z));
        return vec3(src.x, 2.f * tmp / (src.z + tmp + 1e-20f), src.z + tmp);
    }

    static vec4 HSLToHSV(vec4 src)
    {
        return vec4(HSLToHSV(src.rgb), src.a);
    }

    /*
     * Convert linear RGB [0 1] to CIE XYZ [0 100]
     */
    static vec3 LinearRGBToCIEXYZ(vec3 src)
    {
        mat3 m(vec3(41.24f, 21.26f,  1.93f),
               vec3(35.76f, 71.52f, 11.92f),
               vec3(18.05f,  7.22f, 95.05f));
        return m * src;
    }

    static vec4 LinearRGBToCIEXYZ(vec4 src)
    {
        return vec4(LinearRGBToCIEXYZ(src.rgb), src.a);
    }

    /*
     * Convert CIE XYZ [0 100] to linear RGB [0 1]
     */
    static vec3 CIEXYZToLinearRGB(vec3 src)
    {
        mat3 m(vec3( 0.032406f, -0.009689f,  0.000557f),
               vec3(-0.015372f,  0.018758f, -0.002040f),
               vec3(-0.004986f,  0.000415f,  0.010570f));
        return m * src;
    }

    static vec4 CIEXYZToLinearRGB(vec4 src)
    {
        return vec4(CIEXYZToLinearRGB(src.rgb), src.a);
    }

    /*
     * Convert CIE XYZ to CIE xyY
     */
    static vec3 CIEXYZToCIExyY(vec3 src)
    {
        float tmp = 1.0f / (src.x + src.y + src.z);
        return vec3(src.x * tmp, src.y * tmp, src.y);
    }

    static vec4 CIEXYZToCIExyY(vec4 src)
    {
        return vec4(CIEXYZToCIExyY(src.rgb), src.a);
    }

    /*
     * Convert CIE xyY to CIE XYZ
     */
    static vec3 CIExyYToCIEXYZ(vec3 src)
    {
        return src.z * vec3(src.x / src.y,
                            1.0f,
                            (1.0f - src.x - src.y) / src.y);
    }

    static vec4 CIExyYToCIEXYZ(vec4 src)
    {
        return vec4(CIEXYZToCIExyY(src.rgb), src.a);
    }

    /*
     * Convert CIE XYZ to CIE L*a*b*
     * Note: XYZ values are normalised using a D65 illuminant if
     * no white value is provided.
     */
    static vec3 CIEXYZToCIELab(vec3 src, vec3 white_value)
    {
        float const a = (6.0f * 6.0f * 6.0f) / (29 * 29 * 29);
        float const b = (29.0f * 29.0f) / (3 * 6 * 6);
        float const c = 4.0f / 29;
        float const d = 1.0f / 3;

        src /= white_value;

        vec3 f = b * src + vec3(c);
        if (src.x > a)
            f.x = pow(src.x, d);
        if (src.y > a)
            f.y = pow(src.y, d);
        if (src.z > a)
            f.z = pow(src.z, d);

        return vec3(116.f * f.y - 16.f,
                    500.f * (f.x - f.y),
                    200.f * (f.y - f.z));
    }

    static vec3 CIEXYZToCIELab(vec3 src)
    {
        return CIEXYZToCIELab(src, vec3(95.047f, 100.f, 108.883f));
    }

    static vec4 CIEXYZToCIELab(vec4 src, vec4 white_value)
    {
        return vec4(CIEXYZToCIELab(src.rgb, white_value.rgb), src.a);
    }

    static vec4 CIEXYZToCIELab(vec4 src)
    {
        return vec4(CIEXYZToCIELab(src.rgb), src.a);
    }

    /*
     * Return distance using the CIEDE2000 metric,
     * input should be in CIE L*a*b*.
     */
    static float DistanceCIEDE2000(vec3 lab1, vec3 lab2);

    /*
     * Convert a wavelength into an xyY fully saturated colour.
     */
    static vec3 WavelengthToCIExyY(float nm);

    /*
    * Convert uint color to vec4.
    */
    static vec4 FromRGBA32(uint32_t c);
    static vec4 FromRGB32(uint32_t c);
    static vec4 FromRGBA16(uint16_t c);
    static vec4 FromRGB16(uint16_t c);

    /*
    * Convert uint color to vec4.
    */
    static uint32_t ToRGBA32(vec4 c);
    static uint32_t ToRGB32(vec4 c);
    static uint16_t ToRGBA16(vec4 c);
    static uint16_t ToRGB16(vec4 c);

    /*
    * Convert colors to hex strings.
    */
    static String HexString4Bpp(vec3 c);
    static String HexString4Bpp(vec4 c);
    static String HexString8Bpp(vec3 c);
    static String HexString8Bpp(vec4 c);

    /*
    * Convert hex strings to colors.
    */
    static vec4 C8BppHexString(String c);

    /*
    * Some predefined colours
     */
    static const vec4 black, red, green, yellow, blue, magenta,
                      cyan, white, dark_gray, gray, light_gray;
};

} /* namespace lol */

