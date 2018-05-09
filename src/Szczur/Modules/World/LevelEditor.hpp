
//#ifdef EDITOR
#pragma once

#include "Entity.hpp"
#include "Scene.hpp"
#include "Components/ArmatureComponent.hpp"
#include "Components/SpriteComponent.hpp"
#include "data/SpriteDisplayData.hpp"
#include "data/ArmatureDisplayData.hpp"

#include <boost/container/flat_map.hpp>
#include "Szczur/Modules/Input/InputManager.hpp"
#include "Szczur/Modules/Camera/Camera.hpp"
#include "SceneManager.hpp"

namespace rat {
    struct FreeCamera {
        glm::vec3 position{0.f, 0.f, 0.f};
        glm::vec3 rotation{0.f, 0.f, 0.f};
        bool rotating{false};
        float velocity{50.f};
        sf::Vector2i previousMouse{0, 0};
        void move(const glm::vec3& offset) {position += offset;}
        void rotate(const glm::vec3& offset) {rotation += offset;}
        void processEvents(InputManager& input);
    };

    class LevelEditor {
    public:
        LevelEditor(SceneManager& scenes);

        void render(sf3d::RenderTarget& target);
        void update(InputManager& input, Camera& camera);
    private:
        SceneManager& _scenes;

        void _processEventsForFreeCamera(InputManager& input);

        void _renderBar();
        void _renderDisplayDataManager();
        void _renderArmatureDisplayManager();
        void _renderFocusedObjectsParams();
        void _renderObjectsList();
        void _renderComponentsManager();

        bool _ifRenderObjectsList{true};
        bool _ifRenderDisplayDataManager{false};
        bool _ifRenderArmatureDisplayManager{false};
        bool _anySelected{false};
        bool _ifRenderComponentsManager{false};
        size_t _focusedObject{static_cast<size_t>(-1)};
        FreeCamera _freeCamera;
        
    };

    
}

//#endif