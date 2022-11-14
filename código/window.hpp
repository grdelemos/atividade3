#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;

  glm::ivec2 m_viewportSize{};

  Model m_model;
  Model satelite1;
  Model probe;
  Model ship;
  Model station;
  Model ufo;
  Model planet;

  glm::vec3 eye{0.0f, 0.0f, 0.0f};
  glm::vec3 at{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  float lastX = 300, lastY = 300;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float sensitivity = 0.1f;
  glm::ivec2 mousePosition;

  struct Star {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  struct SpaceBody {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  SpaceBody m_satelite1;
  SpaceBody m_probe;
  SpaceBody m_ship;
  SpaceBody m_station;
  SpaceBody m_ufo;
  SpaceBody m_planet;

  std::array<Star, 1000> m_stars;

  float m_angle{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{30.0f};

  GLuint m_program{};

  void randomizeStar(Star &star);
  bool control = false;
  void checkCollision();
};

#endif