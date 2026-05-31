-- Arkheon Simulation Technologies
-- Mission animation loop for the student hand-pose animation.

local animationCode = "Hand Pose Cycle"
local entityAnimationState = {}

local function activateHandPoseAnimation(entityId)
    if animation == nil or animation.setAnimation == nil then
        return false
    end

    local ok = animation.setAnimation(entityId, animationCode, true, 1.0)
    if mission ~= nil and mission.log ~= nil then
        if ok then
            mission.log("Activated animation: " .. animationCode)
        else
            mission.log("Failed to activate animation: " .. animationCode)
        end
    end
    return ok
end

function onInit(entityId)
    entityAnimationState[entityId] = {
        animationApplied = false
    }
    entityAnimationState[entityId].animationApplied = activateHandPoseAnimation(entityId)
end

function onTick(entityId, simulationTimeSeconds, deltaTimeSeconds)
    local state = entityAnimationState[entityId]
    if state == nil then
        state = {
            animationApplied = false
        }
        entityAnimationState[entityId] = state
    end

    if not state.animationApplied then
        state.animationApplied = activateHandPoseAnimation(entityId)
    end
end

function onShutdown(entityId)
    entityAnimationState[entityId] = nil
end
