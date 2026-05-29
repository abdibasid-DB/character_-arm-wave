-- Arkheon Simulation Technologies
-- Proprietary and Confidential.
-- Unauthorized copying of this file, via any medium, is strictly prohibited.
-- (c) Arkheon Simulation Technologies. All rights reserved.

local animationCode = "Arm Wave"

local function activateArmWave(entityId)
    if animation == nil or animation.setAnimation == nil then
        return false
    end

    return animation.setAnimation(entityId, animationCode, true, 1.0)
end

function onInit(entityId)
    activateArmWave(entityId)
end

function onTick(entityId, simulationTimeSeconds, deltaTimeSeconds)
    if animation == nil or animation.getActiveAnimation == nil then
        return
    end

    if animation.getActiveAnimation(entityId) ~= animationCode then
        activateArmWave(entityId)
    end
end

function onShutdown(entityId)
    if animation ~= nil and animation.stop ~= nil then
        animation.stop(entityId)
    end
end
