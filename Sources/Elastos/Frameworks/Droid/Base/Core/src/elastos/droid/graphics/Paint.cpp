//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPaintFontMetrics.h"
#include "elastos/droid/graphics/CPaintFontMetricsInt.h"
#include "elastos/droid/graphics/Shader.h"
#include "elastos/droid/graphics/ColorFilter.h"
#include "elastos/droid/graphics/Xfermode.h"
#include "elastos/droid/graphics/PathEffect.h"
#include "elastos/droid/graphics/MaskFilter.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/graphics/Rasterizer.h"
#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/MinikinSkia.h"
#include "elastos/droid/graphics/MinikinUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/utility/logging/Logger.h>
#include <hwui/utils/Blur.h>
#include <minikin/FontFamily.h>
#include <minikin/Layout.h>
#include <minikin/GraphemeBreak.h>
#include <skia/core/SkPaint.h>
#include <skia/core/SkRasterizer.h>
#include <skia/effects/SkBlurDrawLooper.h>
#include <unicode/uloc.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::IGraphicsOperations;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Core::Character;
using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using android::Layout;
using android::GraphemeBreak;

namespace Elastos {
namespace Droid {
namespace Graphics {

class GetTextFunctor {
public:
    GetTextFunctor(
        /* [in] */ const Layout& layout,
        /* [in] */ SkPath* path,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ NativePaint* paint,
        /* [in] */ uint16_t* glyphs,
        /* [in] */ SkPoint* pos)
        : layout(layout), path(path), x(x), y(y), paint(paint), glyphs(glyphs), pos(pos)
    {
    }

    void operator()(size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            glyphs[i] = layout.getGlyphId(i);
            pos[i].fX = x + layout.getX(i);
            pos[i].fY = y + layout.getY(i);
        }
        if (start == 0) {
            paint->getPosTextPath(glyphs + start, (end - start) << 1, pos + start, path);
        } else {
            paint->getPosTextPath(glyphs + start, (end - start) << 1, pos + start, &tmpPath);
            path->addPath(tmpPath);
        }
    }
private:
    const Layout& layout;
    SkPath* path;
    Float x;
    Float y;
    NativePaint* paint;
    uint16_t* glyphs;
    SkPoint* pos;
    SkPath tmpPath;
};

static const String TAG("Paint");

static void DefaultSettingsForElastos(SkPaint* paint)
{
    // GlyphID encoding is required because we are using Harfbuzz shaping
    paint->setTextEncoding(SkPaint::kGlyphID_TextEncoding);
}

const PaintStyle Paint::sStyleArray[] = {
    PaintStyle_FILL, PaintStyle_STROKE, PaintStyle_FILL_AND_STROKE
};
const PaintCap Paint::sCapArray[] = {
    PaintCap_BUTT, PaintCap_ROUND, PaintCap_SQUARE
};
const PaintJoin Paint::sJoinArray[] = {
    PaintJoin_MITER, PaintJoin_ROUND, PaintJoin_BEVEL
};
const PaintAlign Paint::sAlignArray[] = {
    PaintAlign_LEFT, PaintAlign_CENTER, PaintAlign_RIGHT
};

AutoPtr<ILocale> Paint::GetDefaultLocale()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    return locale;
}

const Int32 Paint::DEFAULT_PAINT_FLAGS = IPaint::DEV_KERN_TEXT_FLAG | IPaint::EMBEDDED_BITMAP_TEXT_FLAG;
const Int32 Paint::BIDI_MAX_FLAG_VALUE = IPaint::BIDI_FORCE_RTL;
const Int32 Paint::BIDI_FLAG_MASK = 0x7;
const Int32 Paint::CURSOR_OPT_MAX_VALUE = IPaint::CURSOR_AT;

CAR_INTERFACE_IMPL(Paint, Object, IPaint);

Paint::Paint()
    : mNativePaint(0)
    , mBidiFlags(IPaint::BIDI_DEFAULT_LTR)
    , mHasCompatScaling(FALSE)
    , mCompatScaling(0)
    , mInvCompatScaling(0)
{}

Paint::~Paint()
{
    NativeFinalizer(mNativePaint);
}

ECode Paint::constructor()
{
    return constructor(0);
}

ECode Paint::constructor(
    /* [in] */ Int32 flags)
{
    mNativePaint = NativeInit();
    SetFlags(flags | DEFAULT_PAINT_FLAGS);
    // TODO: Turning off hinting has undesirable side effects, we need to
    //       revisit hinting once we add support for subpixel positioning
    // setHinting(DisplayMetrics.DENSITY_DEVICE >= DisplayMetrics.DENSITY_TV
    //        ? HINTING_OFF : HINTING_ON);
    mCompatScaling = mInvCompatScaling = 1;
    AutoPtr<ILocale> locale = GetDefaultLocale();
    SetTextLocale(locale);
    return NOERROR;
}

ECode Paint::constructor(
    /* [in] */ IPaint* paint)
{
    VALIDATE_NOT_NULL(paint);
    Paint* paint_ = (Paint*)paint;
    mNativePaint = NativeInitWithPaint(paint_->mNativePaint);
    SetClassVariablesFrom(paint_);
    return NOERROR;
}

ECode Paint::Reset()
{
    NativeReset(mNativePaint);
    SetFlags(DEFAULT_PAINT_FLAGS);

    // TODO: Turning off hinting has undesirable side effects, we need to
    //       revisit hinting once we add support for subpixel positioning
    // setHinting(DisplayMetrics.DENSITY_DEVICE >= DisplayMetrics.DENSITY_TV
    //        ? HINTING_OFF : HINTING_ON);

    mColorFilter = NULL;
    mMaskFilter = NULL;
    mPathEffect = NULL;
    mRasterizer = NULL;
    mShader = NULL;
    mTypeface = NULL;
    mXfermode = NULL;

    mHasCompatScaling = FALSE;
    mCompatScaling = 1;

    mInvCompatScaling = 1;

    mBidiFlags = IPaint::BIDI_DEFAULT_LTR;
    AutoPtr<ILocale> locale = GetDefaultLocale();
    SetTextLocale(locale);
    SetElegantTextHeight(FALSE);
    mFontFeatureSettings = NULL;
    return NOERROR;
}

ECode Paint::Set(
    /* [in] */ IPaint* src)
{
    VALIDATE_NOT_NULL(src)
    Paint* src_ = (Paint*)src;
    if (this != src_) {
        assert(src_ != NULL);
        // copy over the native settings
        NativeSet(mNativePaint, src_->mNativePaint);
        SetClassVariablesFrom(src_);
    }
    return NOERROR;
}

void Paint::SetClassVariablesFrom(
    /* [in] */ Paint* paint)
{
    mColorFilter = paint->mColorFilter;
    mMaskFilter = paint->mMaskFilter;
    mPathEffect = paint->mPathEffect;
    mRasterizer = paint->mRasterizer;

    mShader = NULL;
    if (paint->mShader != NULL) {
        ((Shader*)paint->mShader.Get())->Copy((IShader**)&mShader);
    }

    mTypeface = paint->mTypeface;
    mXfermode = paint->mXfermode;

    mHasCompatScaling = paint->mHasCompatScaling;
    mCompatScaling = paint->mCompatScaling;
    mInvCompatScaling = paint->mInvCompatScaling;

    mBidiFlags = paint->mBidiFlags;
    mLocale = paint->mLocale;
    mFontFeatureSettings = paint->mFontFeatureSettings;
}

ECode Paint::SetCompatibilityScaling(
    /* [in] */ Float factor)
{
    if (factor == 1.0) {
        mHasCompatScaling = FALSE;
        mCompatScaling = mInvCompatScaling = 1.0f;
    }
    else {
        mHasCompatScaling = TRUE;
        mCompatScaling = factor;
        mInvCompatScaling = 1.0f/factor;
    }
    return NOERROR;
}

ECode Paint::GetBidiFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mBidiFlags;
    return NOERROR;
}

ECode Paint::SetBidiFlags(
    /* [in] */ Int32 flags)
{
    // only flag value is the 3-bit BIDI control setting
    flags &= BIDI_FLAG_MASK;
    if (flags > BIDI_MAX_FLAG_VALUE) {
        Logger::E(TAG, "IllegalArgumentException unknown bidi flag: %d", flags);
        // throw new IllegalArgumentException("unknown bidi flag: " + flags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBidiFlags = flags;
    return NOERROR;
}

ECode Paint::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *flags = paint->getFlags();
    return NOERROR;
}

ECode Paint::SetFlags(
    /* [in] */ Int32 flags)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setFlags(flags);
    return NOERROR;
}

ECode Paint::GetHinting(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *mode = paint->getHinting() == SkPaint::kNo_Hinting ? 0 : 1;
    return NOERROR;
}

ECode Paint::SetHinting(
    /* [in] */ Int32 mode)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setFlags(mode == 0 ? SkPaint::kNo_Hinting : SkPaint::kSlight_Hinting);
    return NOERROR;
}

ECode Paint::IsAntiAlias(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::ANTI_ALIAS_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetAntiAlias(
    /* [in] */ Boolean aa)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setAntiAlias(aa);
    return NOERROR;
}

ECode Paint::IsDither(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::DITHER_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetDither(
    /* [in] */ Boolean dither)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setDither(dither);
    return NOERROR;
}

ECode Paint::IsLinearText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::LINEAR_TEXT_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetLinearText(
    /* [in] */ Boolean linearText)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setLinearText(linearText);
    return NOERROR;
}

ECode Paint::IsSubpixelText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::SUBPIXEL_TEXT_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetSubpixelText(
    /* [in] */ Boolean subpixelText)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setSubpixelText(subpixelText);
    return NOERROR;
}

ECode Paint::IsUnderlineText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::UNDERLINE_TEXT_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetUnderlineText(
    /* [in] */ Boolean underlineText)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setUnderlineText(underlineText);
    return NOERROR;
}

ECode Paint::IsStrikeThruText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::STRIKE_THRU_TEXT_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetStrikeThruText(
    /* [in] */ Boolean strikeThruText)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setStrikeThruText(strikeThruText);
    return NOERROR;
}

ECode Paint::IsFakeBoldText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::FAKE_BOLD_TEXT_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetFakeBoldText(
    /* [in] */ Boolean fakeBoldText)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setFakeBoldText(fakeBoldText);
    return NOERROR;
}

ECode Paint::IsFilterBitmap(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    GetFlags(&flags);
    *result = (flags & IPaint::FILTER_BITMAP_FLAG) != 0;
    return NOERROR;
}

ECode Paint::SetFilterBitmap(
    /* [in] */ Boolean filterBitmap)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setFilterBitmap(filterBitmap);
    return NOERROR;
}

ECode Paint::GetStyle(
    /* [out] */ PaintStyle* style)
{
    VALIDATE_NOT_NULL(style)
    *style = sStyleArray[NativeGetStyle(mNativePaint)];
    return NOERROR;
}

ECode Paint::SetStyle(
    /* [in] */ PaintStyle style)
{
    NativeSetStyle(mNativePaint, style);
    return NOERROR;
}

ECode Paint::GetColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *color = paint->getColor();
    return NOERROR;
}

ECode Paint::SetColor(
    /* [in] */ Int32 color)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setColor(color);
    return NOERROR;
}

ECode Paint::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha)
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *alpha = paint->getAlpha();
    return NOERROR;
}

ECode Paint::SetAlpha(
    /* [in] */ Int32 a)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->setAlpha(a);
    return NOERROR;
}

ECode Paint::SetARGB(
    /* [in] */ Int32 a,
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return SetColor((a << 24) | (r << 16) | (g << 8) | b);
}

ECode Paint::GetStrokeWidth(
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *width = SkScalarToFloat(paint->getStrokeWidth());
    return NOERROR;
}

ECode Paint::SetStrokeWidth(
    /* [in] */ Float width)
{
    GraphicsNative::GetNativePaint(this)->setStrokeWidth(width);
    return NOERROR;
}

ECode Paint::GetStrokeMiter(
    /* [out] */ Float* miter)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *miter = SkScalarToFloat(paint->getStrokeMiter());
    return NOERROR;
}

ECode Paint::SetStrokeMiter(
    /* [in] */ Float miter)
{
    GraphicsNative::GetNativePaint(this)->setStrokeMiter(miter);
    return NOERROR;
}

ECode Paint::GetStrokeCap(
    /* [out] */ PaintCap* cap)
{
    VALIDATE_NOT_NULL(cap)
    *cap = sCapArray[NativeGetStrokeCap(mNativePaint)];
    return NOERROR;
}

ECode Paint::SetStrokeCap(
    /* [in] */ PaintCap cap)
{
    NativeSetStrokeCap(mNativePaint, cap);
    return NOERROR;
}

ECode Paint::GetStrokeJoin(
    /* [out] */ PaintJoin* join)
{
    VALIDATE_NOT_NULL(join)
    *join = sJoinArray[NativeGetStrokeJoin(mNativePaint)];
    return NOERROR;
}

ECode Paint::SetStrokeJoin(
    /* [in] */ PaintJoin join)
{
    NativeSetStrokeJoin(mNativePaint, join);
    return NOERROR;
}

ECode Paint::GetFillPath(
    /* [in] */ IPath* src,
    /* [in] */ IPath* dst,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NativeGetFillPath(mNativePaint, ((CPath*)src)->mNativePath, ((CPath*)dst)->mNativePath);
    return NOERROR;
}

ECode Paint::GetShader(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader)
    *shader = mShader;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

ECode Paint::SetShader(
    /* [in] */ IShader* shader)
{
    Int64 shaderNative = 0;
    if (shader != NULL) {
        Shader* s = (Shader*)shader;
        assert(s != NULL);
        shaderNative = s->mNativeInstance;
    }
    NativeSetShader(mNativePaint, shaderNative);
    mShader = shader;
    return NOERROR;
}

ECode Paint::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mColorFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode Paint::SetColorFilter(
    /* [in] */ IColorFilter* filter)
{
    Int64 filterNative = 0;
    if (filter != NULL) {
        ColorFilter* cf = (ColorFilter*)filter;
        assert(cf != NULL);
        filterNative = cf->mNativeInstance;
    }
    NativeSetColorFilter(mNativePaint, filterNative);
    mColorFilter = filter;
    return NOERROR;
}

ECode Paint::GetXfermode(
    /* [out] */ IXfermode** xfermode)
{
    VALIDATE_NOT_NULL(xfermode)
    *xfermode = mXfermode;
    REFCOUNT_ADD(*xfermode);
    return NOERROR;
}

ECode Paint::SetXfermode(
    /* [in] */ IXfermode* xfermode)
{
    Int64 xfermodeNative = 0;
    if (xfermode != NULL) {
        Xfermode* x = (Xfermode*)xfermode;
        assert(x != NULL);
        xfermodeNative = x->mNativeInstance;
    }
    NativeSetXfermode(mNativePaint, xfermodeNative);
    mXfermode = xfermode;
    return NOERROR;
}

ECode Paint::GetPathEffect(
    /* [out] */ IPathEffect** effect)
{
    VALIDATE_NOT_NULL(effect)
    *effect = mPathEffect;
    REFCOUNT_ADD(*effect);
    return NOERROR;
}

ECode Paint::SetPathEffect(
    /* [in] */ IPathEffect* effect)
{
    Int64 effectNative = 0;
    if (effect != NULL) {
        PathEffect* pe = (PathEffect*)effect;
        assert(pe != NULL);
        effectNative = pe->mNativeInstance;
    }
    NativeSetPathEffect(mNativePaint, effectNative);
    mPathEffect = effect;
    return NOERROR;
}

ECode Paint::GetMaskFilter(
    /* [out] */ IMaskFilter** maskfilter)
{
    VALIDATE_NOT_NULL(maskfilter)
    *maskfilter = mMaskFilter;
    REFCOUNT_ADD(*maskfilter)
    return NOERROR;
}

ECode Paint::SetMaskFilter(
    /* [in] */ IMaskFilter* maskfilter)
{
    Int64 maskfilterNative = 0;
    if (maskfilter != NULL) {
        MaskFilter* mf = (MaskFilter*)maskfilter;
        assert(mf != NULL);
        maskfilterNative = mf->mNativeInstance;
    }
    NativeSetMaskFilter(mNativePaint, maskfilterNative);
    mMaskFilter = maskfilter;
    return NOERROR;
}

ECode Paint::GetTypeface(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = mTypeface;
    REFCOUNT_ADD(*typeface);
    return NOERROR;
}

Int64 Paint::GetNativeTypeface()
{
    if (mTypeface != NULL) {
        return ((Typeface*)mTypeface.Get())->mNativeInstance;
    }
    return 0;
}

ECode Paint::SetTypeface(
    /* [in] */ ITypeface* typeface)
{
    mTypeface = typeface;
    Int64 typefaceNative = GetNativeTypeface();
    NativeSetTypeface(mNativePaint, typefaceNative);
    return NOERROR;
}

ECode Paint::GetRasterizer(
    /* [out] */ IRasterizer** rasterizer)
{
    VALIDATE_NOT_NULL(rasterizer)
    *rasterizer = mRasterizer;
    REFCOUNT_ADD(*rasterizer);
    return NOERROR;
}

ECode Paint::SetRasterizer(
    /* [in] */ IRasterizer* rasterizer)
{
    Int64 rasterizerNative = 0;
    if (rasterizer != NULL) {
        Rasterizer* r = (Rasterizer*)rasterizer;
        assert(r != NULL);
        rasterizerNative = r->mNativeInstance;
    }
    NativeSetRasterizer(mNativePaint, rasterizerNative);
    mRasterizer = rasterizer;
    return NOERROR;
}

ECode Paint::SetShadowLayer(
    /* [in] */ Float radius,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int32 shadowColor)
{
    NativeSetShadowLayer(mNativePaint, radius, dx, dy, shadowColor);
    return NOERROR;
}

ECode Paint::ClearShadowLayer()
{
    return SetShadowLayer(0, 0, 0, 0);
}

ECode Paint::HasShadowLayer(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = NativeHasShadowLayer(mNativePaint);
    return NOERROR;
}

ECode Paint::GetTextAlign(
    /* [out] */ PaintAlign* align)
{
    VALIDATE_NOT_NULL(align)
    *align = sAlignArray[NativeGetTextAlign(mNativePaint)];
    return NOERROR;
}

ECode Paint::SetTextAlign(
    /* [in] */ PaintAlign align)
{
    NativeSetTextAlign(mNativePaint, align);
    return NOERROR;
}

ECode Paint::GetTextLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode Paint::SetTextLocale(
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        Logger::E(TAG, "locale cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isEqual;
    if (locale->Equals(mLocale, &isEqual), isEqual) return NOERROR;
    mLocale = locale;
    String str;
    locale->ToString(&str);
    NativeSetTextLocale(mNativePaint, str);
    return NOERROR;
}

ECode Paint::IsElegantTextHeight(
    /* [out] */ Boolean* isElegantTextHeight)
{
    VALIDATE_NOT_NULL(isElegantTextHeight);
    *isElegantTextHeight = GraphicsNative::GetNativePaint(this)->getFontVariant() == android::VARIANT_ELEGANT;
    return NOERROR;
}

ECode Paint::SetElegantTextHeight(
    /* [in] */ Boolean elegant)
{
    GraphicsNative::GetNativePaint(this)->setFontVariant(elegant ? android::VARIANT_ELEGANT : android::VARIANT_DEFAULT);
    return NOERROR;
}

ECode Paint::GetTextSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *size = SkScalarToFloat(paint->getTextSize());
    return NOERROR;
}

ECode Paint::SetTextSize(
    /* [in] */ Float textSize)
{
    GraphicsNative::GetNativePaint(this)->setTextSize(textSize);
    return NOERROR;
}

ECode Paint::GetTextScaleX(
    /* [out] */ Float* scaleX)
{
    VALIDATE_NOT_NULL(scaleX);
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *scaleX = SkScalarToFloat(paint->getTextScaleX());
    return NOERROR;
}

ECode Paint::SetTextScaleX(
    /* [in] */ Float scaleX)
{
    GraphicsNative::GetNativePaint(this)->setTextScaleX(scaleX);
    return NOERROR;
}

ECode Paint::GetTextSkewX(
    /* [out] */ Float* skewX)
{
    VALIDATE_NOT_NULL(skewX)
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    *skewX = SkScalarToFloat(paint->getTextSkewX());
    return NOERROR;
}

ECode Paint::SetTextSkewX(
    /* [in] */ Float skewX)
{
    GraphicsNative::GetNativePaint(this)->setTextSkewX(skewX);
    return NOERROR;
}

ECode Paint::GetLetterSpacing(
    /* [out] */ Float* spacing)
{
    VALIDATE_NOT_NULL(spacing);
    *spacing = NativeGetLetterSpacing(mNativePaint);
    return NOERROR;
}

ECode Paint::SetLetterSpacing(
    /* [in] */ Float letterSpacing)
{
    NativeSetLetterSpacing(mNativePaint, letterSpacing);
    return NOERROR;
}

ECode Paint::GetFontFeatureSettings(
    /* [out] */ String* settings)
{
    VALIDATE_NOT_NULL(settings);
    *settings = mFontFeatureSettings;
    return NOERROR;
}

ECode Paint::SetFontFeatureSettings(
    /* [in] */ const String& _settings)
{
    String settings = _settings;
    if (/*settings != null && */settings.Equals(String(""))) {
        settings = NULL;
    }
    if ((settings == NULL && mFontFeatureSettings == NULL)
            || (settings != NULL && settings.Equals(mFontFeatureSettings))) {
        return NOERROR;
    }
    mFontFeatureSettings = settings;
    NativeSetFontFeatureSettings(mNativePaint, settings);
    return NOERROR;
}

ECode Paint::Ascent(
    /* [out] */ Float* distance)
{
    VALIDATE_NOT_NULL(distance)
    SkPaint::FontMetrics metrics;
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->getFontMetrics(&metrics);
    *distance = SkScalarToFloat(metrics.fAscent);
    return NOERROR;
}

ECode Paint::Descent(
    /* [out] */ Float* distance)
{
    VALIDATE_NOT_NULL(distance)
    SkPaint::FontMetrics metrics;
    NativePaint* paint = reinterpret_cast<NativePaint*>(mNativePaint);
    paint->getFontMetrics(&metrics);
    *distance = SkScalarToFloat(metrics.fDescent);
    return NOERROR;
}

static SkScalar getMetricsInternal(
    /* [in] */ Paint* epaint,
    /* [in] */ NativePaint::FontMetrics *metrics)
{
    const int kElegantTop = 2500;
    const int kElegantBottom = -1000;
    const int kElegantAscent = 1900;
    const int kElegantDescent = -500;
    const int kElegantLeading = 0;
    NativePaint* paint = reinterpret_cast<NativePaint*>(epaint->mNativePaint);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(epaint->GetNativeTypeface());
    typeface = TypefaceImpl_resolveDefault(typeface);
    android::FakedFont baseFont = typeface->fFontCollection->baseFontFaked(typeface->fStyle);
    float saveSkewX = paint->getTextSkewX();
    bool savefakeBold = paint->isFakeBoldText();
    MinikinFontSkia::populateSkPaint(paint, baseFont.font, baseFont.fakery);
    SkScalar spacing = paint->getFontMetrics(metrics);
    // The populateSkPaint call may have changed fake bold / text skew
    // because we want to measure with those effects applied, so now
    // restore the original settings.
    paint->setTextSkewX(saveSkewX);
    paint->setFakeBoldText(savefakeBold);
    if (paint->getFontVariant() == android::VARIANT_ELEGANT) {
        SkScalar size = paint->getTextSize();
        metrics->fTop = -size * kElegantTop / 2048;
        metrics->fBottom = -size * kElegantBottom / 2048;
        metrics->fAscent = -size * kElegantAscent / 2048;
        metrics->fDescent = -size * kElegantDescent / 2048;
        metrics->fLeading = size * kElegantLeading / 2048;
        spacing = metrics->fDescent - metrics->fAscent + metrics->fLeading;
    }
    return spacing;
}

ECode Paint::GetFontMetrics(
    /* [in] */ IPaintFontMetrics* metrics,
    /* [out] */ Float* spacing)
{
    VALIDATE_NOT_NULL(spacing)
    SkPaint::FontMetrics metrics_;
    SkScalar spacing_ = getMetricsInternal(this, &metrics_);

    if (metrics) {
        metrics->SetTop(SkScalarToFloat(metrics_.fTop));
        metrics->SetAscent(SkScalarToFloat(metrics_.fAscent));
        metrics->SetDescent(SkScalarToFloat(metrics_.fDescent));
        metrics->SetBottom(SkScalarToFloat(metrics_.fBottom));
        metrics->SetLeading(SkScalarToFloat(metrics_.fLeading));
    }
    *spacing = SkScalarToFloat(spacing_);
    return NOERROR;
}

ECode Paint::GetFontMetrics(
    /* [out] */ IPaintFontMetrics** metrics)
{
    CPaintFontMetrics::New(metrics);
    Float spacing;
    return GetFontMetrics(*metrics, &spacing);
}

ECode Paint::GetFontMetricsInt(
    /* [in] */ IPaintFontMetricsInt* metricsObj,
    /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing);
    // NPE_CHECK_RETURN_ZERO(env, paint);
    NativePaint::FontMetrics metrics;

    getMetricsInternal(this, &metrics);
    int ascent = SkScalarRoundToInt(metrics.fAscent);
    int descent = SkScalarRoundToInt(metrics.fDescent);
    int leading = SkScalarRoundToInt(metrics.fLeading);

    if (metricsObj) {
        SkASSERT(metricsObj);
        metricsObj->SetTop(SkScalarFloorToInt(metrics.fTop));
        metricsObj->SetAscent(ascent);
        metricsObj->SetDescent(descent);
        metricsObj->SetBottom(SkScalarCeilToInt(metrics.fBottom));
        metricsObj->SetLeading(leading);
    }
    *spacing = descent - ascent + leading;
    return NOERROR;
}

ECode Paint::GetFontMetricsInt(
    /* [out] */ IPaintFontMetricsInt** fmi)
{
    VALIDATE_NOT_NULL(fmi)
    CPaintFontMetricsInt::New(fmi);
    Int32 spacing;
    return GetFontMetricsInt(*fmi, &spacing);
}

ECode Paint::GetFontSpacing(
    /* [out] */ Float* spacing)
{
    return GetFontMetrics(NULL, spacing);
}

ECode Paint::MeasureText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 0;
    if ((index | count) < 0 || index + count > text->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text->GetLength() == 0 || count == 0) {
        *width = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *width = (Float) Elastos::Core::Math::Ceil(NativeMeasureText(text, index, count, mBidiFlags));
        return NOERROR;
    }
    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Float w = NativeMeasureText(text, index, count, mBidiFlags);
    SetTextSize(oldSize);
    *width = (Float) Elastos::Core::Math::Ceil(w * mInvCompatScaling);
    return NOERROR;
}

ECode Paint::MeasureText(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 0;
    if (text.IsNull()) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text.GetLength() == 0 || start == end) {
        *width = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *width = (Float) Elastos::Core::Math::Ceil(NativeMeasureText(text, start, end, mBidiFlags));
        return NOERROR;
    }

    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Float w = NativeMeasureText(text, start, end, mBidiFlags);
    SetTextSize(oldSize);
    *width = (Float) Elastos::Core::Math::Ceil(w * mInvCompatScaling);
    return NOERROR;
}

ECode Paint::MeasureText(
    /* [in] */ const String& text,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 0;
    if (text.IsNull()) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (text.IsEmpty()) {
        *width = 0;
        return NOERROR;
    }

    if (!mHasCompatScaling) {
        *width = (Float) Elastos::Core::Math::Ceil(NativeMeasureText(text, mBidiFlags));
        return NOERROR;
    }
    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Float w = NativeMeasureText(text, mBidiFlags);
    SetTextSize(oldSize);
    *width = (Float) Elastos::Core::Math::Ceil(w * mInvCompatScaling);
    return NOERROR;
}

ECode Paint::MeasureText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 0;
    if (text == NULL) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 length;
    text->GetLength(&length);
    if ((start | end | (end - start) | (length - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (length == 0 || start == end) {
        *width = 0;
        return NOERROR;
    }
    // if (text instanceof String) {
    //     return measureText((String)text, start, end);
    // }
    // if (text instanceof SpannedString ||
    //     text instanceof SpannableString) {
    //     return measureText(text.toString(), start, end);
    // }
    // if (text instanceof GraphicsOperations) {
    //     return ((GraphicsOperations)text).measureText(start, end, this);
    // }

    AutoPtr< ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);
    TextUtils::GetChars(text, start, end, buf, 0);
    ECode ec = MeasureText(buf, 0, end - start, width);
    TemporaryBuffer::Recycle(buf);
    return ec;
}

ECode Paint::BreakText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (index < 0 || text->GetLength() - index < Elastos::Core::Math::Abs(count)) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text->GetLength() == 0 || count == 0) {
        *number = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *number = NativeBreakText(mNativePaint, GetNativeTypeface(), text, index, count, maxWidth, mBidiFlags, measuredWidth);
        return NOERROR;
    }

    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Int32 res = NativeBreakText(mNativePaint, GetNativeTypeface(), text, index, count, maxWidth * mCompatScaling, mBidiFlags,
            measuredWidth);
    SetTextSize(oldSize);
    if (measuredWidth != NULL) (*measuredWidth)[0] *= mInvCompatScaling;
    *number = res;
    return NOERROR;
}

ECode Paint::BreakText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean measureForwards,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (text == NULL) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 length;
    text->GetLength(&length);
    if ((start | end | (end - start) | (length - end)) < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (length == 0 || start == end) {
        *number = 0;
        return NOERROR;
    }
    // if (start == 0 && text instanceof String && end == text.length()) {
    //     return breakText((String) text, measureForwards, maxWidth,
    //                      measuredWidth);
    // }

    AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);

    TextUtils::GetChars(text, start, end, buf, 0);

    ECode ec;
    if (measureForwards) {
        ec = BreakText(buf, 0, end - start, maxWidth, measuredWidth, number);
    }
    else {
        ec = BreakText(buf, 0, -(end - start), maxWidth, measuredWidth, number);
    }

    TemporaryBuffer::Recycle(buf);
    return ec;
}

ECode Paint::BreakText(
    /* [in] */ const String& text,
    /* [in] */ Boolean measureForwards,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (text.IsNull()) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (text.IsEmpty()) {
        *number = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *number = NativeBreakText(mNativePaint, GetNativeTypeface(), text, measureForwards, maxWidth, mBidiFlags, measuredWidth);
        return NOERROR;
    }

    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Int32 res = NativeBreakText(mNativePaint, GetNativeTypeface(), text, measureForwards, maxWidth * mCompatScaling,
            mBidiFlags, measuredWidth);
    SetTextSize(oldSize);
    if (measuredWidth != NULL) (*measuredWidth)[0] *= mInvCompatScaling;
    *number = res;
    return NOERROR;
}

ECode Paint::GetTextWidths(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [out] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if ((index | count) < 0 || index + count > text->GetLength()
            || count > widths->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text->GetLength() == 0 || count == 0) {
        *number = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *number = NativeGetTextWidths(mNativePaint, GetNativeTypeface(), text, index, count, mBidiFlags, widths);
        return NOERROR;
    }

    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Int32 res = NativeGetTextWidths(mNativePaint, GetNativeTypeface(), text, index, count, mBidiFlags, widths);
    SetTextSize(oldSize);
    for (Int32 i = 0; i < res; i++) {
        (*widths)[i] *= mInvCompatScaling;
    }
    *number = res;
    return NOERROR;
}

ECode Paint::GetTextWidths(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (text == NULL) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 length;
    text->GetLength(&length);
    if ((start | end | (end - start) | (length - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (end - start > widths->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (length == 0 || start == end) {
        *number = 0;
        return NOERROR;
    }
    // if (text instanceof String) {
    //     return getTextWidths((String) text, start, end, widths);
    // }
    // if (text instanceof SpannedString ||
    //     text instanceof SpannableString) {
    //     return getTextWidths(text.toString(), start, end, widths);
    // }
    // if (text instanceof GraphicsOperations) {
    //     return ((GraphicsOperations) text).getTextWidths(start, end,
    //                                                          widths, this);
    // }

    AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);
    TextUtils::GetChars(text, start, end, buf, 0);
    ECode ec = GetTextWidths(buf, 0, end - start, widths, number);
    TemporaryBuffer::Recycle(buf);
    return ec;
}

ECode Paint::GetTextWidths(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (text.IsNull()) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (end - start > widths->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text.GetLength() == 0 || start == end) {
        *number = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *number = NativeGetTextWidths(mNativePaint, GetNativeTypeface(), text, start, end, mBidiFlags, widths);
        return NOERROR;
    }
    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Int32 res = NativeGetTextWidths(mNativePaint, GetNativeTypeface(), text, start, end, mBidiFlags, widths);
    SetTextSize(oldSize);
    for (Int32 i = 0; i < res; i++) {
        (*widths)[i] *= mInvCompatScaling;
    }
    *number = res;
    return NOERROR;
}

ECode Paint::GetTextWidths(
    /* [in] */ const String& text,
    /* [in] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    return GetTextWidths(text, 0, text.GetLength(), widths, number);
}

ECode Paint::GetTextRunAdvances(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    VALIDATE_NOT_NULL(advance)
    *advance = 0;
    if ((index | count | contextIndex | contextCount | advancesIndex
            | (index - contextIndex) | (contextCount - count)
            | ((contextIndex + contextCount) - (index + count))
            | (chars->GetLength() - (contextIndex + contextCount))
            | (advances == NULL ? 0 :
                (advances->GetLength() - (advancesIndex + count)))) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (chars->GetLength() == 0 || count == 0) {
        *advance = 0;
        return NOERROR;
    }
    if (!mHasCompatScaling) {
        *advance = NativeGetTextRunAdvances(mNativePaint, GetNativeTypeface(), chars, index, count,
                contextIndex, contextCount, isRtl, advances, advancesIndex);
        return NOERROR;
    }

    Float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Float res = NativeGetTextRunAdvances(mNativePaint, GetNativeTypeface(), chars, index, count,
                contextIndex, contextCount, isRtl, advances, advancesIndex);
    SetTextSize(oldSize);

    if (advances != NULL) {
        for (Int32 i = advancesIndex, e = i + count; i < e; i++) {
            (*advances)[i] *= mInvCompatScaling;
        }
    }
    *advance = res * mInvCompatScaling; // assume errors are not significant
    return NOERROR;
}

ECode Paint::GetTextRunAdvances(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    VALIDATE_NOT_NULL(advance)
    *advance = 0;
    if (text == NULL) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 length;
    text->GetLength(&length);
    if ((start | end | contextStart | contextEnd | advancesIndex | (end - start)
            | (start - contextStart) | (contextEnd - end)
            | (length - contextEnd)
            | (advances == NULL ? 0 :
                (advances->GetLength() - advancesIndex - (end - start)))) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

//  if (text instanceof String) {
//        return getTextRunAdvances((String) text, start, end,
//                contextStart, contextEnd, isRtl, advances, advancesIndex);
//    }
//    if (text instanceof SpannedString ||
//        text instanceof SpannableString) {
//        return getTextRunAdvances(text.toString(), start, end,
//                contextStart, contextEnd, isRtl, advances, advancesIndex);
//  }
//    if (text instanceof GraphicsOperations) {
//        return ((GraphicsOperations) text).getTextRunAdvances(start, end,
//                contextStart, contextEnd, flags, advances, advancesIndex, this);
//    }
    if (length == 0 || end == start) {
        *advance = 0;
        return NOERROR;
    }

    Int32 contextLen = contextEnd - contextStart;
    Int32 len = end - start;
    AutoPtr< ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(contextLen);
    TextUtils::GetChars(text, contextStart, contextEnd, buf, 0);
    ECode ec = GetTextRunAdvances(buf, start - contextStart, len,
            0, contextLen, isRtl, advances, advancesIndex, advance);
    TemporaryBuffer::Recycle(buf);
    return ec;
}

ECode Paint::GetTextRunAdvances(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    VALIDATE_NOT_NULL(advance)
    *advance = 0;
    if (text.IsNull()) {
        Logger::E(TAG, "text cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((start | end | contextStart | contextEnd | advancesIndex | (end - start)
            | (start - contextStart) | (contextEnd - end)
            | (text.GetLength() - contextEnd)
            | (advances == NULL ? 0 :
                (advances->GetLength() - advancesIndex - (end - start)))) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (text.GetLength() == 0 || start == end) {
        *advance = 0;
        return NOERROR;
    }

    if (!mHasCompatScaling) {
        *advance = NativeGetTextRunAdvances(mNativePaint, GetNativeTypeface(), text, start, end,
                contextStart, contextEnd, isRtl, advances, advancesIndex);
        return NOERROR;
    }

    float oldSize;
    GetTextSize(&oldSize);
    SetTextSize(oldSize * mCompatScaling);
    Float totalAdvance = NativeGetTextRunAdvances(mNativePaint, GetNativeTypeface(), text, start, end,
                contextStart, contextEnd, isRtl, advances, advancesIndex);
    SetTextSize(oldSize);

   if (advances != NULL) {
        for (Int32 i = advancesIndex, e = i + (end - start); i < e; i++) {
            (*advances)[i] *= mInvCompatScaling;
        }
    }

    *advance = totalAdvance * mInvCompatScaling; // assume errors are insignificant
    return NOERROR;
}

ECode Paint::GetTextRunCursor(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextLength,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    *position = 0;
    Int32 contextEnd = contextStart + contextLength;
    if (((contextStart | contextEnd | offset | (contextEnd - contextStart)
        | (offset - contextStart) | (contextEnd - offset)
        | (text->GetLength() - contextEnd) | cursorOpt) < 0)
        || cursorOpt > CURSOR_OPT_MAX_VALUE) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *position = NativeGetTextRunCursor(mNativePaint, text,
        contextStart, contextLength, dir, offset, cursorOpt);
    return NOERROR;
}

ECode Paint::GetTextRunCursor(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    *position = 0;
   if (IString::Probe(text) || ISpannedString::Probe(text) || ISpannableString::Probe(text)) {
        String str;
        text->ToString(&str);
       return GetTextRunCursor(str, contextStart, contextEnd,
               dir, offset, cursorOpt, position);
   }
   if (IGraphicsOperations::Probe(text)) {
       return IGraphicsOperations::Probe(text)->GetTextRunCursor(
               contextStart, contextEnd, dir, offset, cursorOpt, this, position);
   }

    Int32 contextLen = contextEnd - contextStart;
    AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(contextLen);
    TextUtils::GetChars(text, contextStart, contextEnd, buf, 0);
    ECode ec = GetTextRunCursor(buf, 0, contextLen, dir, offset - contextStart, cursorOpt, position);
    TemporaryBuffer::Recycle(buf);
    return ec;
}

ECode Paint::GetTextRunCursor(
    /* [in] */ const String& text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    *position = 0;
    if (((contextStart | contextEnd | offset | (contextEnd - contextStart)
            | (offset - contextStart) | (contextEnd - offset)
            | (text.GetLength() - contextEnd) | cursorOpt) < 0)
            || cursorOpt > CURSOR_OPT_MAX_VALUE) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *position = NativeGetTextRunCursor(mNativePaint, text,
            contextStart, contextEnd, dir, offset, cursorOpt);
    return NOERROR;
}

ECode Paint::GetTextPath(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPath* path)
{
    if ((index | count) < 0 || index + count > text->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    NativeGetTextPath(mNativePaint, GetNativeTypeface(), mBidiFlags, text, index, count, x, y, ((CPath*)path)->mNativePath);
    return NOERROR;
}

ECode Paint::GetTextPath(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPath* path)
{
    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    NativeGetTextPath(mNativePaint, GetNativeTypeface(), mBidiFlags, text, start, end, x, y, ((CPath*)path)->mNativePath);
    return NOERROR;
}

ECode Paint::GetTextBounds(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IRect* bounds)
{
    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (bounds == NULL) {
//        throw new NullPointerException("need bounds Rect");
        return E_NULL_POINTER_EXCEPTION;
    }
    NativeGetStringBounds(mNativePaint, GetNativeTypeface(), text, start, end, mBidiFlags, bounds);
    return NOERROR;
}

ECode Paint::GetTextBounds(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ IRect* bounds)
{
    if ((index | count) < 0 || index + count > text->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (bounds == NULL) {
//        throw new NullPointerException("need bounds Rect");
        return E_NULL_POINTER_EXCEPTION;
    }
    NativeGetCharArrayBounds(mNativePaint, GetNativeTypeface(), text, index, count, mBidiFlags, bounds);
    return NOERROR;
}

Int64 Paint::NativeInit()
{
    NativePaint* obj = new NativePaint();
    DefaultSettingsForElastos(obj);
    return reinterpret_cast<Int64>(obj);
}

Int64 Paint::NativeInitWithPaint(
    /* [in] */ Int64 paintHandle)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    NativePaint* obj = new NativePaint(*paint);
    return reinterpret_cast<Int64>(obj);
}

void Paint::NativeReset(
    /* [in] */ Int64 nObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    paint->reset();
    DefaultSettingsForElastos(paint);
}

void Paint::NativeSet(
    /* [in] */ Int64 nDst,
    /* [in] */ Int64 nSrc)
{
    *(SkPaint*)nDst = *(SkPaint*)nSrc;
}

Int32 Paint::NativeGetStyle(
    /* [in] */ Int64 nObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    return paint->getStyle();
}

void Paint::NativeSetStyle(
    /* [in] */ Int64 nObj,
    /* [in] */ PaintStyle style)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    paint->setStyle((SkPaint::Style)style);
}

Int32 Paint::NativeGetStrokeCap(
    /* [in] */ Int64 nObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    return paint->getStrokeCap();
}

void Paint::NativeSetStrokeCap(
    /* [in] */ Int64 nObj,
    /* [in] */ PaintCap cap)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    paint->setStrokeCap((SkPaint::Cap)cap);
}

Int32 Paint::NativeGetStrokeJoin(
    /* [in] */ Int64 nObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    return paint->getStrokeJoin();
}

void Paint::NativeSetStrokeJoin(
    /* [in] */ Int64 nObj,
    /* [in] */ PaintJoin join)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    paint->setStrokeJoin((SkPaint::Join)join);
}

Boolean Paint::NativeGetFillPath(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 src,
    /* [in] */ Int64 dst)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    return paint->getFillPath(*(SkPath*)src, (SkPath*)dst);
}

Int64 Paint::NativeSetShader(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 shaderObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    SkShader* shader = reinterpret_cast<SkShader *>(shaderObj);
    return reinterpret_cast<Int64>(paint->setShader(shader));
}

Int64 Paint::NativeSetColorFilter(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 filterObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    SkColorFilter* filter = reinterpret_cast<SkColorFilter *>(filterObj);
    return reinterpret_cast<Int64>(paint->setColorFilter(filter));
}

Int64 Paint::NativeSetXfermode(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 xfermodeObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    SkXfermode* xfermode = reinterpret_cast<SkXfermode *>(xfermodeObj);
    return reinterpret_cast<Int64>(paint->setXfermode(xfermode));
}

Int64 Paint::NativeSetPathEffect(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 effectObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    SkPathEffect* effect = reinterpret_cast<SkPathEffect *>(effectObj);
    return reinterpret_cast<Int64>(paint->setPathEffect(effect));
}

Int64 Paint::NativeSetMaskFilter(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 maskfilterObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    SkMaskFilter* maskfilter = reinterpret_cast<SkMaskFilter *>(maskfilterObj);
    return reinterpret_cast<Int64>(paint->setMaskFilter(maskfilter));
}

Int64 Paint::NativeSetTypeface(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 typeface)
{
    // TODO: in Paint refactoring, set typeface on android Paint, not Paint
    return 0;
}

Int64 Paint::NativeSetRasterizer(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 rasterizerHandle)
{
    NativePaint* obj = reinterpret_cast<NativePaint*>(objHandle);
    SkAutoTUnref<SkRasterizer> rasterizer(GraphicsNative::RefNativeRasterizer(rasterizerHandle));
    return reinterpret_cast<Int64>(obj->setRasterizer(rasterizer));
}

Int32 Paint::NativeGetTextAlign(
    /* [in] */ Int64 nObj)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    return paint->getTextAlign();
}

void Paint::NativeSetTextAlign(
    /* [in] */ Int64 nObj,
    /* [in] */ PaintAlign align)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(nObj);
    paint->setTextAlign((SkPaint::Align)align);
}

// generate bcp47 identifier for the supplied locale
static void ToLanguageTag(
    /* [in] */ char* output,
    /* [in] */ size_t outSize,
    /* [in] */ const char* locale)
{
    if (output == NULL || outSize <= 0) {
        return;
    }
    if (locale == NULL) {
        output[0] = '\0';
        return;
    }
    char canonicalChars[ULOC_FULLNAME_CAPACITY];
    UErrorCode uErr = U_ZERO_ERROR;
    uloc_canonicalize(locale, canonicalChars, ULOC_FULLNAME_CAPACITY,
            &uErr);
    if (U_SUCCESS(uErr)) {
        char likelyChars[ULOC_FULLNAME_CAPACITY];
        uErr = U_ZERO_ERROR;
        uloc_addLikelySubtags(canonicalChars, likelyChars,
                ULOC_FULLNAME_CAPACITY, &uErr);
        if (U_SUCCESS(uErr)) {
            uErr = U_ZERO_ERROR;
            uloc_toLanguageTag(likelyChars, output, outSize, FALSE, &uErr);
            if (U_SUCCESS(uErr)) {
                return;
            }
            else {
                // ALOGD("uloc_toLanguageTag(\"%s\") failed: %s", likelyChars,
                //         u_errorName(uErr));
            }
        }
        else {
            // ALOGD("uloc_addLikelySubtags(\"%s\") failed: %s",
            //         canonicalChars, u_errorName(uErr));
        }
    }
    else {
        // ALOGD("uloc_canonicalize(\"%s\") failed: %s", locale,
        //         u_errorName(uErr));
    }
    // unable to build a proper language identifier
    output[0] = '\0';
}

void Paint::NativeSetTextLocale(
    /* [in] */ Int64 objHandle,
    /* [in] */ const String& locale)
{
    NativePaint* obj = reinterpret_cast<NativePaint*>(objHandle);
    // ScopedUtfChars localeChars(env, locale);
    char langTag[ULOC_FULLNAME_CAPACITY];
    ToLanguageTag(langTag, ULOC_FULLNAME_CAPACITY, locale.string());

    obj->setTextLocale(String(langTag));
}

Float Paint::NativeMeasureText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 bidiFlags)
{
    // NPE_CHECK_RETURN_ZERO(env, text);
    if (text == NULL) {
        return 0;
    }

    size_t textLength = text->GetLength();
    if ((index | count) < 0 || (size_t)(index + count) > textLength) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }
    if (count == 0) {
        return 0;
    }

    NativePaint* paint = GraphicsNative::GetNativePaint(this);
    Float result = 0;

    AutoPtr< ArrayOf<Char16> > textArray = ArrayUtils::ToChar16Array(text);

    Layout layout;
    TypefaceImpl* typeface = GraphicsNative::GetNativeTypeface(this);
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, textArray->GetPayload(),
        index, count, textLength);
    result = layout.getAdvance();
    return result;
}

Float Paint::NativeMeasureText(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bidiFlags)
{
    size_t textLength = text.GetLength();
    int count = end - start;
    if ((start | count) < 0 || (size_t)end > textLength) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }
    if (count == 0) {
        return 0;
    }

    NativePaint* paint = GraphicsNative::GetNativePaint(this);
    Float width = 0;
    AutoPtr<ArrayOf<Char16> > textArray = text.GetChar16s();

    Layout layout;
    TypefaceImpl* typeface = GraphicsNative::GetNativeTypeface(this);
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, textArray->GetPayload(),
        start, count, textLength);
    width = layout.getAdvance();
    return width;
}

Float Paint::NativeMeasureText(
    /* [in] */ const String& text,
    /* [in] */ Int32 bidiFlags)
{
    size_t textLength = text.GetLength();
    if (textLength == 0) {
        return 0;
    }

    NativePaint* paint = GraphicsNative::GetNativePaint(this);
    Float width = 0;
    AutoPtr<ArrayOf<Char16> > textArray = text.GetChar16s();

    Layout layout;
    TypefaceImpl* typeface = GraphicsNative::GetNativeTypeface(this);
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, textArray->GetPayload(),
        0, textLength, textLength);
    width = layout.getAdvance();
    return width;
}

static int breakText(
    /* [in] */ const NativePaint& paint,
    /* [in] */ TypefaceImpl* typeface,
    /* [in] */ const Char16* text,
    /* [in] */ int count,
    /* [in] */ float maxWidth,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ ArrayOf<Float>* measuredArray,
    /* [in] */ NativePaint::TextBufferDirection textBufferDirection)
{
    size_t measuredCount = 0;
    float measured = 0;

    Layout layout;
    MinikinUtils::doLayout(&layout, &paint, bidiFlags, typeface, text, 0, count, count);
    float* advances = new float[count];
    layout.getAdvances(advances);
    const bool forwardScan = (textBufferDirection == NativePaint::kForward_TextBufferDirection);
    for (int i = 0; i < count; i++) {
        // traverse in the given direction
        int index = forwardScan ? i : (count - i - 1);
        float width = advances[index];
        if (measured + width > maxWidth) {
            break;
        }
        // properly handle clusters when scanning backwards
        if (forwardScan || width != 0.0f) {
            measuredCount = i + 1;
        }
        measured += width;
    }
    delete[] advances;

    if (measuredArray && measuredArray->GetLength() > 0) {
        AutoFloatArray autoMeasured(measuredArray, 1);
        float* array = autoMeasured.ptr();
        array[0] = measured;
    }
    return measuredCount;
}

Int32 Paint::NativeBreakText(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float maxWidth,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ ArrayOf<Float>* measuredWidth)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);

    NativePaint::TextBufferDirection tbd;
    if (count < 0) {
        tbd = NativePaint::kBackward_TextBufferDirection;
        count = -count;
    }
    else {
        tbd = NativePaint::kForward_TextBufferDirection;
    }

    if ((index < 0) || (index + count > text->GetLength())) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }

    AutoPtr< ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text, index, count);

    count = breakText(*paint, typeface, char16Array->GetPayload(),
        count, maxWidth, bidiFlags, measuredWidth, tbd);
    return count;
}

Int32 Paint::NativeBreakText(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ const String& text,
    /* [in] */ Boolean forwards,
    /* [in] */ Float maxWidth,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ ArrayOf<Float>* measuredWidth)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);

    NativePaint::TextBufferDirection tbd = forwards ?
                                    NativePaint::kForward_TextBufferDirection :
                                    NativePaint::kBackward_TextBufferDirection;

    int count = text.GetLength();
    AutoPtr< ArrayOf<Char16> > char16Array = text.GetChar16s();
    count = breakText(*paint, typeface, char16Array->GetPayload(),
        count, maxWidth, bidiFlags, measuredWidth, tbd);
    // env->ReleaseStringChars(jtext, text);
    return count;
}

static int dotextwidths(
    /* [in] */ NativePaint* paint,
    /* [in] */ TypefaceImpl* typeface,
    /* [in] */ const Char16* text,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* widths,
    /* [in] */ Int32 bidiFlags)
{
    // NPE_CHECK_RETURN_ZERO(env, paint);
    // NPE_CHECK_RETURN_ZERO(env, text);
    if (paint == NULL || text == NULL) {
        return 0;
    }

    if (count < 0 || !widths) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }
    if (count == 0) {
        return 0;
    }
    size_t widthsLength = widths->GetLength();
    if ((size_t)count > widthsLength) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }

    AutoFloatArray autoWidths(widths, count);
    Float* widthsArray = autoWidths.ptr();

    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, 0, count, count);
    layout.getAdvances(widthsArray);

    return count;
}

Int32 Paint::NativeGetTextWidths(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 bidiFlags,
    /* [out] */ ArrayOf<Float>* widths)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    AutoPtr<ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text);
    return dotextwidths(paint, typeface, char16Array->GetPayload() + index, count, widths, bidiFlags);
}

Int32 Paint::NativeGetTextWidths(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bidiFlags,
    /* [out] */ ArrayOf<Float>* widths)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    AutoPtr<ArrayOf<Char16> > textArray = text.GetChar16s();
    return dotextwidths(paint, typeface,
        textArray->GetPayload() + start, end - start, widths, bidiFlags);
}

Int32 Paint::NativeGetTextGlyphs(
    /* [in] */ Int64 nObj,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [out] */ ArrayOf<Char32>* glyphs)
{
    assert(0 && "TODO: need jni codes.");
    return 0;
}

static Float doTextRunAdvances(
    /* [in] */ NativePaint *paint,
    /* [in] */ TypefaceImpl* typeface,
    /* [in] */ const Char16 *text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextCount,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex)
{
    // NPE_CHECK_RETURN_ZERO(env, paint);
    // NPE_CHECK_RETURN_ZERO(env, text);
    if (paint == NULL || text == NULL) {
        return 0;
    }

    if ((start | count | contextCount | advancesIndex) < 0 || contextCount < count) {
        // doThrowAIOOBE(env);
        assert(0);
        return 0;
    }
    if (count == 0) {
        return 0;
    }
    if (advances) {
        size_t advancesLength = advances->GetLength();
        if ((size_t)count > advancesLength) {
            // doThrowAIOOBE(env);
            assert(0);
            return 0;
        }
    }
    AutoPtr<ArrayOf<Float> > advancesArray = ArrayOf<Float>::Alloc(count);
    Float totalAdvance = 0;

    int bidiFlags = isRtl ? kBidi_Force_RTL : kBidi_Force_LTR;

    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, start, count, contextCount);
    layout.getAdvances(advancesArray->GetPayload());
    totalAdvance = layout.getAdvance();

    if (advances != NULL) {
        advances->Copy(advancesIndex, advancesArray, count);
    }
    return totalAdvance;
}

Float Paint::NativeGetTextRunAdvances(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text);
    Float result = doTextRunAdvances(paint, typeface, char16Array->GetPayload() + contextIndex,
        index - contextIndex, count, contextCount, isRtl, advances, advancesIndex);
    return result;
}

Float Paint::NativeGetTextRunAdvances(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean isRtl,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);

    AutoPtr< ArrayOf<Char16> > char16Array = text.GetChar16s();
    Float result = doTextRunAdvances(paint, typeface, char16Array->GetPayload() + contextStart,
        start - contextStart, end - start, contextEnd - contextStart, isRtl,
        advances, advancesIndex);
    return result;
}

enum MoveOpt {
    AFTER, AT_OR_AFTER, BEFORE, AT_OR_BEFORE, AT
};

static Int32 doTextRunCursor(
    /* [in] */ NativePaint* paint,
    /* [in] */ const Char16 *text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 opt)
{
    GraphemeBreak::MoveOpt moveOpt = GraphemeBreak::MoveOpt(opt);
    size_t result = GraphemeBreak::getTextRunCursor(text, start, count, offset, moveOpt);
    return static_cast<Int32>(result);
}

Int32 Paint::NativeGetTextRunCursor(
    /* [in] */ Int64 paintHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextCount,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text);
    Int32 result = doTextRunCursor(paint, char16Array->GetPayload(),
        contextStart, contextCount, dir, offset, cursorOpt);
    return result;
}

Int32 Paint::NativeGetTextRunCursor(
    /* [in] */ Int64 paintHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = text.GetChar16s();
    Int32 result = doTextRunCursor(paint, char16Array->GetPayload(), contextStart,
            contextEnd - contextStart, dir, offset, cursorOpt);
    return result;
}

static void getTextPath(
    /* [in] */ NativePaint* paint,
    /* [in] */ TypefaceImpl* typeface,
    /* [in] */ const Char16* text,
    /* [in] */ Int32 count,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ SkPath* path)
{
    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, 0, count, count);
    size_t nGlyphs = layout.nGlyphs();
    uint16_t* glyphs = new uint16_t[nGlyphs];
    SkPoint* pos = new SkPoint[nGlyphs];

    x += MinikinUtils::xOffsetForTextAlign(paint, layout);
    NativePaint::Align align = paint->getTextAlign();
    paint->setTextAlign(NativePaint::kLeft_Align);
    paint->setTextEncoding(NativePaint::kGlyphID_TextEncoding);
    GetTextFunctor f(layout, path, x, y, paint, glyphs, pos);
    MinikinUtils::forFontRun(layout, paint, f);
    paint->setTextAlign(align);
    delete[] glyphs;
    delete[] pos;
}

void Paint::NativeGetTextPath(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int64 pathHandle)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text);
    getTextPath(paint, typeface, char16Array->GetPayload() + index, count, bidiFlags, x, y, path);
}

void Paint::NativeGetTextPath(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int64 pathHandle)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = text.GetChar16s();
    getTextPath(paint, typeface, char16Array->GetPayload() + start, end - start, bidiFlags, x, y, path);
}

static void doTextBounds(
    /* [in] */ ArrayOf<Char16>* text,
    /* [in] */ Int32 count,
    /* [in] */ IRect* bounds,
    /* [in] */ NativePaint* paint,
    /* [in] */ TypefaceImpl* typeface,
    /* [in] */ Int32 bidiFlags)
{
    SkRect  r;
    SkIRect ir;

    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface,
        text->GetPayload(), 0, count, count);
    MinikinRect rect;
    layout.getBounds(&rect);
    r.fLeft = rect.mLeft;
    r.fTop = rect.mTop;
    r.fRight = rect.mRight;
    r.fBottom = rect.mBottom;
    r.roundOut(&ir);
    GraphicsNative::SkIRect2IRect(ir, bounds);
}

void Paint::NativeGetStringBounds(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ IRect* bounds)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = text.GetChar16s(start, end);
    doTextBounds(char16Array, char16Array->GetLength(), bounds, paint, typeface, bidiFlags);
}

void Paint::NativeGetCharArrayBounds(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ IRect* bounds)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    AutoPtr< ArrayOf<Char16> > char16Array = ArrayUtils::ToChar16Array(text, index, count);
    doTextBounds(char16Array, char16Array->GetLength(), bounds, paint, typeface, bidiFlags);
}

void Paint::NativeFinalizer(
    /* [in] */ Int64 objHandle)
{
    NativePaint* obj = reinterpret_cast<NativePaint*>(objHandle);
    delete obj;
}

void Paint::NativeSetShadowLayer(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Float radius,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int32 color)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    if (radius <= 0) {
        paint->setLooper(NULL);
    }
    else {
        SkScalar sigma = android::uirenderer::Blur::convertRadiusToSigma(radius);
        paint->setLooper(SkBlurDrawLooper::Create((SkColor)color, sigma, dx, dy))->unref();
    }
}

Boolean Paint::NativeHasShadowLayer(
    /* [in] */ Int64 paintHandle)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    return paint->getLooper() && paint->getLooper()->asABlurShadow(NULL);
}

Float Paint::NativeGetLetterSpacing(
    /* [in] */ Int64 paintHandle)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    return paint->getLetterSpacing();
}

void Paint::NativeSetLetterSpacing(
    /* [in] */ Int64 paintHandle,
    /* [in] */ Float letterSpacing)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    paint->setLetterSpacing(letterSpacing);
}

void Paint::NativeSetFontFeatureSettings(
    /* [in] */ Int64 paintHandle,
    /* [in] */ const String& settings)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintHandle);
    if (settings.IsNull()) {
        paint->setFontFeatureSettings(String(NULL));
    }
    else {
        // ScopedUtfChars settingsChars(env, settings);
        paint->setFontFeatureSettings(settings);
    }
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
