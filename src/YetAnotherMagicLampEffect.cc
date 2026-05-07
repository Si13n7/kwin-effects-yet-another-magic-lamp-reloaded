/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// Own
#include "YetAnotherMagicLampEffect.h"
#include "Model.h"

// Auto-generated
#include "YetAnotherMagicLampConfig.h"

// kwineffects
#include <core/renderviewport.h>

// Qt
#include <QTimer>

// std
#include <cmath>

enum ShapeCurve {
    Linear = 0,
    Quad = 1,
    Cubic = 2,
    Quart = 3,
    Quint = 4,
    Sine = 5,
    Circ = 6,
    Bounce = 7,
    Bezier = 8
};

YetAnotherMagicLampEffect::YetAnotherMagicLampEffect()
{
    reconfigure(ReconfigureAll);

    for (KWin::EffectWindow *w : KWin::effects->stackingOrder()) {
        slotWindowAdded(w);
    }

    connect(KWin::effects, &KWin::EffectsHandler::windowAdded,
        this, &YetAnotherMagicLampEffect::slotWindowAdded);
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted,
        this, &YetAnotherMagicLampEffect::slotWindowDeleted);
    connect(KWin::effects, &KWin::EffectsHandler::activeFullScreenEffectChanged,
        this, &YetAnotherMagicLampEffect::slotActiveFullScreenEffectChanged);
}

YetAnotherMagicLampEffect::~YetAnotherMagicLampEffect()
{
}

void YetAnotherMagicLampEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    YetAnotherMagicLampConfig::self()->read();

    QEasingCurve curve;
    const auto shapeCurve = static_cast<ShapeCurve>(YetAnotherMagicLampConfig::shapeCurve());
    switch (shapeCurve) {
    case ShapeCurve::Linear:
        curve.setType(QEasingCurve::Linear);
        break;

    case ShapeCurve::Quad:
        curve.setType(QEasingCurve::InOutQuad);
        break;

    case ShapeCurve::Cubic:
        curve.setType(QEasingCurve::InOutCubic);
        break;

    case ShapeCurve::Quart:
        curve.setType(QEasingCurve::InOutQuart);
        break;

    case ShapeCurve::Quint:
        curve.setType(QEasingCurve::InOutQuint);
        break;

    case ShapeCurve::Sine:
        curve.setType(QEasingCurve::InOutSine);
        break;

    case ShapeCurve::Circ:
        curve.setType(QEasingCurve::InOutCirc);
        break;

    case ShapeCurve::Bounce:
        curve.setType(QEasingCurve::InOutBounce);
        break;

    case ShapeCurve::Bezier:
        // With the cubic bezier curve, "0" corresponds to the furtherst edge
        // of a window, "1" corresponds to the closest edge.
        curve.setType(QEasingCurve::BezierSpline);
        curve.addCubicBezierSegment(
            QPointF(0.3, 0.0),
            QPointF(0.7, 1.0),
            QPointF(1.0, 1.0));
        break;
    default:
        // Fallback to the sine curve.
        curve.setType(QEasingCurve::InOutSine);
        break;
    }
    m_modelParameters.shapeCurve = curve;

    const int baseDuration = animationTime<YetAnotherMagicLampConfig>(std::chrono::milliseconds(300));
    m_modelParameters.squashDuration = std::chrono::milliseconds(baseDuration);
    m_modelParameters.stretchDuration = std::chrono::milliseconds(qMax(qRound(baseDuration * 0.7), 1));
    m_modelParameters.bumpDuration = std::chrono::milliseconds(baseDuration);
    m_modelParameters.shapeFactor = YetAnotherMagicLampConfig::initialShapeFactor();
    m_modelParameters.bumpDistance = YetAnotherMagicLampConfig::maxBumpDistance();

    m_gridResolution = YetAnotherMagicLampConfig::gridResolution();
}

void YetAnotherMagicLampEffect::prePaintScreen(KWin::ScreenPrePaintData& data, std::chrono::milliseconds presentTime)
{
    for (AnimationData& animData : m_animations) {
        animData.model.advance(presentTime);
    }

    data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;

    KWin::effects->prePaintScreen(data, presentTime);
}

void YetAnotherMagicLampEffect::postPaintScreen()
{
    for (auto it = m_animations.begin(); it != m_animations.end();) {
        if (it->model.done()) {
            unredirect(it.key());
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }

    KWin::effects->addRepaintFull();
    KWin::effects->postPaintScreen();
}

void YetAnotherMagicLampEffect::paintWindow(const KWin::RenderTarget& renderTarget,
                                             const KWin::RenderViewport& viewport,
                                             KWin::EffectWindow* w, int mask,
                                             const KWin::Region& region,
                                             KWin::WindowPaintData& data)
{
    auto it = m_animations.constFind(w);
    if (it != m_animations.constEnd() && it->model.needsClip()) {
        const KWin::Region clipLogical(it->model.clipRegion());
        const KWin::Region clip = viewport.mapToDeviceCoordinatesAligned(clipLogical);
        KWin::effects->paintWindow(renderTarget, viewport, w, mask, clip, data);
        return;
    }

    KWin::effects->paintWindow(renderTarget, viewport, w, mask, region, data);
}

void YetAnotherMagicLampEffect::apply(KWin::EffectWindow* window, int mask, KWin::WindowPaintData& data, KWin::WindowQuadList& quads)
{
    Q_UNUSED(mask)
    Q_UNUSED(data)

    auto it = m_animations.constFind(window);
    if (it == m_animations.constEnd()) {
        return;
    }

    quads = quads.makeGrid(m_gridResolution);
    (*it).model.apply(quads);
}

bool YetAnotherMagicLampEffect::isActive() const
{
    return !m_animations.isEmpty();
}

bool YetAnotherMagicLampEffect::supported()
{
    if (!KWin::effects->animationsSupported()) {
        return false;
    }
    if (KWin::effects->isOpenGLCompositing()) {
        return true;
    }
    return false;
}

void YetAnotherMagicLampEffect::slotWindowAdded(KWin::EffectWindow* w)
{
    connect(w, &KWin::EffectWindow::minimizedChanged,
        this, &YetAnotherMagicLampEffect::slotMinimizedChanged);
}

void YetAnotherMagicLampEffect::slotMinimizedChanged(KWin::EffectWindow* w)
{
    if (w->isMinimized()) {
        startMinimize(w);
    } else {
        startUnminimize(w);
    }
}

void YetAnotherMagicLampEffect::startMinimize(KWin::EffectWindow* w)
{
    if (KWin::effects->activeFullScreenEffect()) {
        return;
    }

    const QRectF iconRect = w->iconGeometry();
    if (!iconRect.isValid()) {
        // On Wayland secondary screens the Plasma taskbar sets iconGeometry
        // asynchronously (after QML layout). Keep the window visible and retry
        // once the geometry arrives.
        if (m_pendingMinimize.contains(w)) {
            return;
        }
        m_pendingMinimize[w] = KWin::EffectWindowVisibleRef(w, KWin::EffectWindow::PAINT_DISABLED_BY_MINIMIZE);
        QPointer<KWin::EffectWindow> weakRef(w);
        QTimer::singleShot(150, this, [this, weakRef]() {
            KWin::EffectWindow* win = weakRef.data();
            m_pendingMinimize.remove(win);
            if (win && win->isMinimized()) {
                startMinimize(win);
            }
        });
        return;
    }

    AnimationData& animData = m_animations[w];
    animData.model.setWindow(w);
    animData.model.setParameters(m_modelParameters);
    animData.model.start(Model::AnimationKind::Minimize);
    animData.visibleRef = KWin::EffectWindowVisibleRef(w, KWin::EffectWindow::PAINT_DISABLED_BY_MINIMIZE);

    redirect(w);

    KWin::effects->addRepaintFull();
}

void YetAnotherMagicLampEffect::startUnminimize(KWin::EffectWindow* w)
{
    if (KWin::effects->activeFullScreenEffect()) {
        return;
    }

    const QRectF iconRect = w->iconGeometry();
    if (!iconRect.isValid()) {
        return;
    }

    AnimationData& animData = m_animations[w];
    animData.model.setWindow(w);
    animData.model.setParameters(m_modelParameters);
    animData.model.start(Model::AnimationKind::Unminimize);

    redirect(w);

    KWin::effects->addRepaintFull();
}

void YetAnotherMagicLampEffect::slotWindowDeleted(KWin::EffectWindow* w)
{
    m_pendingMinimize.remove(w);
    m_animations.remove(w);
}

void YetAnotherMagicLampEffect::slotActiveFullScreenEffectChanged()
{
    if (KWin::effects->activeFullScreenEffect() != nullptr) {
        m_pendingMinimize.clear();
        for (auto it = m_animations.constBegin(); it != m_animations.constEnd(); ++it) {
            unredirect(it.key());
        }
        m_animations.clear();
    }
}
