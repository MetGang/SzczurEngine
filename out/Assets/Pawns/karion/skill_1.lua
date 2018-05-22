-- Skill: Move
local skill = THIS:newSkill("Dash and hit")
THIS:addUsable(skill, 0)

skill:setType("SPACE")

function skill:onProvide(canvas) 
	self:renderCircle(canvas, 20)
end

function skill:onInit()
	self.angle = self:getPawn():getAngleToPointer()
	self.clock = Utility.Clock:new()
	self:getPawn():setFlip(self:getPawn():getDirection(self.angle))
end

function skill:onUpdate(dt)
	self:getPawn():moveInDirection(self.angle, dt*700)
	if self.clock:elapsed()>0.20 or self:getPawn():isCollision() then
		self:getPawn():setFrame(0)
		self:kill()
		self:getPawn():useSkill("Dash and hit : hit")
	elseif self.clock:elapsed()>0.10 then
		self:getPawn():setFrame(3)
	else
		self:getPawn():setFrame(2)
	end
end

-- hit
local hit = THIS:newSkill("Dash and hit : hit")
THIS:addUsable(hit, 1)

function hit:onInit()
	self.clock = Utility.Clock.new()
	local trigger = self:makeTrigger()
	trigger:setDuration(0.1)
	trigger:nearestOnly()
	trigger:setRadius(50)
	trigger:setPosition(self:getPawn():getPosition())
	function trigger:onInit()
		self:getTarget():addHp(-15)
		self.target = self:getTarget()
	end
	function trigger:onUpdate(dt) 
		local angle = self.target:getAngleTo(self:getPosition())-3.14159
		self.target:moveInDirection(angle, dt*500);
	end
end

function hit:onUpdate(dt)
	if self.clock:elapsed()>0.20 then
		self:getPawn():setFrame(0)
		self:kill()
	elseif self.clock:elapsed()>0.10 then
		self:getPawn():setFrame(5)
	else
		self:getPawn():setFrame(4)
	end
end
