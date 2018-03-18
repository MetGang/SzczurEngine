#include "ArmatureObjectType.hpp"

/** @file ArmatureObjectType.hpp
 ** @description Implementation file with Armature Object Type class as shared game armature object type information.
 ** @author Piotr (H4RRY) <piotrkrupa06@gmail.com>
 ** @author Patryk (PsychoX) Ludwikowski <psychoxivi+basementstudios@gmail.com>
 **/

#include <string>
#include <exception>
#include <stdexcept>
#include <array>
#include <vector>
#include <fstream>

#include <dragonBones/SFMLTextureData.h>
#include <dragonBones/SFMLTextureAtlasData.h>

#include "Szczur/JSON.hpp"
#include "Szczur/Debug.hpp"
#include "Szczur/Modules/DragonBones/DragonBones.hpp"

namespace rat
{

/* Properties */ 
const std::string& ArmatureObjectType::getPoseString(ArmatureObjectType::PoseID_t poseID) const
{
	const auto& names = this->getPosesNames();
	if (names.size() > poseID) {
		return names[poseID];
	}
	LOG_WARN("[World][ArmatureObjectType{*", this, ";\"", this->name, "}] ",
		"Pose of ID `", poseID, "` was not found; using default.");
	return names[ArmatureObjectType::defaultPoseID];
}
ArmatureObjectType::PoseID_t ArmatureObjectType::getPoseID(const std::string& poseString) const
{
	const auto& names = this->getPosesNames();
	for (std::vector<std::string>::size_type i = 0, I = names.size(); i < I; i++) {
        if (names[i] == poseString) {
            return i;
        }
    }
    LOG_WARN("[World][ArmatureObjectType{*", this, ";\"", this->name, "}] ",
		"Pose of ID for name `", poseString, "` was not found; using default.");
	return ArmatureObjectType::defaultPoseID;
}

const std::vector<std::string>& ArmatureObjectType::getPosesNames() const
{
	return this->skeletonData->armatureNames;
}



/* Operators */
/// Constructor
ArmatureObjectType::ArmatureObjectType(const std::string& typeName, rat::DragonBones::Factory_t& factory)
	: factory(factory)
{
	this->name = typeName;
	
	try {
		// @todo multi-atlas 

		// Load skeleton data
		this->skeletonData = this->factory.loadDragonBonesData(ArmatureObjectType::armaturesAssetsPath + this->name + ArmatureObjectType::skeletonDataPath, this->name);
		
		if (this->skeletonData == nullptr) {
			throw std::runtime_error("Could not load armature skeleton data.");
		}

		// Load textures data
		this->textures.reserve(99);
		for(auto& armatures : this->skeletonData->armatures) {
			for (auto& skins : armatures.second->skins) {
				for (auto& displays : skins.second->displays) {
					for (auto display : displays.second) {
						// Load the information
						auto textureData = dragonBones::BaseObject::borrowObject<dragonBones::SFMLTextureData>();
						this->texturesData.push_back(textureData);
						textureData->rotated = false; 
						textureData->name = display->name;
						
						// Load the texture
						auto& texture = this->textures.emplace_back();
						LOG_INFO(ArmatureObjectType::armaturesAssetsPath + this->name + ArmatureObjectType::texturesPathPrefix + display->path + ArmatureObjectType::texturesPathSuffix);
						if (!texture.loadFromFile(ArmatureObjectType::armaturesAssetsPath + this->name + ArmatureObjectType::texturesPathPrefix + display->path + ArmatureObjectType::texturesPathSuffix)) {
							throw std::runtime_error("Could not load armature texture: `" + display->path + "`.");
						}
						textureData->setTexture(&texture);	
					}
				}
			}
		}
		
		// Setup atlas data
		this->atlasData = dragonBones::BaseObject::borrowObject<dragonBones::SFMLTextureAtlasData>();
		this->atlasData->name = this->skeletonData->name;
		for (auto& textureData : this->texturesData) {
			textureData->parent = this->atlasData;
			this->atlasData->addTexture(textureData);
		}
		this->factory.addTextureAtlasData(this->atlasData, this->name);
	}
	catch (...) {
		std::throw_with_nested(std::runtime_error("Could not load armature object type: `" + this->name + "`."));
	}
	
	// @todo , togglable states? (grouped textures and visibility switching)?
	
	LOG_INFO("[World][ArmatureObjectType{*", this, ";\"", this->name, "}] ", 
		"Loaded ", this->getPosesNames().size(), " armature poses and ", this->textures.size(), " textures.");
	//LOG_INFO(this->skeletonData->armatureNames.back());
}

ArmatureObjectType::~ArmatureObjectType()
{
	this->factory.removeDragonBonesData(this->skeletonData->name);
	this->factory.removeTextureAtlasData(this->atlasData->name);
	for (auto& textureData : this->texturesData) {
		textureData->returnToPool();
	}
}



/* Methods */
/// createPose
ArmatureObjectType::PosesContainer_t<ArmatureObjectType::Pose_t*> ArmatureObjectType::createPoses() const
{
	ArmatureObjectType::PosesContainer_t<ArmatureObjectType::Pose_t*> poses;
	poses.reserve(this->getPosesNames().size());
	
	for (const auto& name : this->getPosesNames()) {
		poses.push_back(this->factory.buildArmatureDisplay(name));
	}
	
	return poses;
}

}