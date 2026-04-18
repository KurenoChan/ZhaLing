#include <Windows.h> // using Microsoft Windows library
#include <gl/GL.h>	 // OpenGL Library (To interact directly with to GPU, faster) [Code <-> OpenGL (Abstraction on top of many different GPUs) <-> GPU]
#include <gl/GLU.h>
#include <math.h>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <cmath>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

#define CLASS_TITLE "OpenGL Window Class"
#define WINDOW_TITLE "ZhaLing Prototype"

struct Color
{
	float r;
	float g;
	float b;
};

struct ColorList
{
	std::vector<Color> colors;
};

enum CameraMode
{
	PERSPECTIVE,
	ORTHO,
	LSIDE,
	RSIDE,
	CAMERA_COUNT
};

CameraMode currentCameraMode = PERSPECTIVE;

// ===========
// Scene Modes
// ===========
enum SceneMode
{
	INTERACT,
	ANIMATION,
	CHARACTER_CUSTOM,
	WEAPON_CUSTOM
};

SceneMode currentSceneMode = INTERACT;

enum CharacterMode
{
	LENGZAI_MODE,
	CHIBI_MODE
};

struct CharacterTexturePreset
{
	GLuint skin;
	GLuint hair;
	GLuint outfitPrimary;
	GLuint outfitSecondary;
	GLuint accessory;
};

enum FiveElementPreset
{
	FIRE_ELEMENT,
	WATER_ELEMENT,
	WOOD_ELEMENT,
	METAL_ELEMENT,
	EARTH_ELEMENT,
	FIVE_ELEMENT_COUNT
};

CharacterMode currentCharacterMode = LENGZAI_MODE;
std::vector<CharacterTexturePreset> characterTexturePresets;
int currentCharacterTexturePresetIndex = 0;

// ===========
// Camera
// ===========
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;

float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraAngleZ = 0.0f;

// ===========
// Light
// ===========
float lightX = 0.0f;
float lightY = 0.0f;
float lightZ = 0.0f;

float movementFactor = 0.1f;
float rotationFactor = 2.0f;

// Toggle Controls
bool isCameraMode = false;
bool isLightMode = false;

bool isLightOn = true;


float scarfTime = 0.0f;
float scarfSpeed = 0.2f;

// ========================
// MODEL CONTROL
// ========================
// NeZha Character Model
float characterX = 0.0f;
float characterY = 0.0f;
float characterZ = 0.0f;

// Character Parts
struct PartRotation
{
	float angleX, angleY, angleZ;

	float defaultX, defaultY, defaultZ;

	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
};
// ===========
// Weapon States
// ===========
int currentWeapon = 0; // 0 = None, 1 = Spear, 2 = Sword (Add more later)
const int TOTAL_WEAPONS = 4; // Update this number as you add more weapons
float wheelRotationAngle = 0.0f; // Tracks the current spin position
float wheelRotationSpeed = 0.0f; // Tracks how fast it is spinning
int currentBladeIndex = 0;
const int TOTAL_BLADE_TEXTURES = 2; // 0 = Normal Blade, 1 = Red Blade

// =========================
// Weapon ANIMATION VARIABLES
// =========================
int currentWeaponAnim = 0;
const int TOTAL_Weapon_ANIMS = 1; // We will just have 1 (Spear Thrust) for now
bool isPlaying = false;    // Tracks if the animation is playing or paused
float animFrame = 0.0f;    // Tracks the current frame of the animation
bool isLooping = true;     // Tracks if the animation should loop
float animSpeed = 1.0f;    // 1.0 = normal, 0.5 = slow, 2.0 = fast
// ===========
// Grip State
// ===========
float currentGrip = 0.0f;

enum Part
{
	HEAD,

	UPPER_TORSO,
	LOWER_TORSO,

	LEFT_UPPER_ARM,
	LEFT_LOWER_ARM,
	LEFT_HAND,

	RIGHT_UPPER_ARM,
	RIGHT_LOWER_ARM,
	RIGHT_HAND,

	LEFT_UPPER_LEG,
	LEFT_LOWER_LEG,
	LEFT_FOOT,

	RIGHT_UPPER_LEG,
	RIGHT_LOWER_LEG,
	RIGHT_FOOT,

	PART_COUNT
};

PartRotation parts[PART_COUNT];
Part currentPart = HEAD;

float partRotationFactor = 5.0f;

// Textures [Q2]
std::vector<GLuint> textures;
int currentTextureIndex = 0;
GLuint currentTexture;

// -------------------
// Lighting Setup
// -------------------
// LIGHT 1: Default Spotlight
GLfloat light1Ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};	 // ambient = minimum brightness of scene
GLfloat light1Diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	 // diffuse = real lighting that reveals geometry
GLfloat light1Specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // specular = how glossy the surface looks
Color light1Color = {0.8f, 0.8f, 0.8f};

// LIGHT 2: Warm Sunset / Golden Hour
GLfloat light2Ambient[] = {0.2f, 0.1f, 0.1f, 1.0f};	 // Deep reddish shadows
GLfloat light2Diffuse[] = {1.0f, 0.7f, 0.3f, 1.0f};	 // Strong orange/gold light
GLfloat light2Specular[] = {1.0f, 0.9f, 0.7f, 1.0f}; // Bright yellow-white glints
Color light2Color = {1.0f, 0.7f, 0.3f};

// LIGHT 3: Cold Moonlight / Cyberpunk
GLfloat light3Ambient[] = {0.1f, 0.1f, 0.2f, 1.0f};	 // Faint blue ambient
GLfloat light3Diffuse[] = {0.4f, 0.6f, 1.0f, 1.0f};	 // Cool blue-white light
GLfloat light3Specular[] = {0.8f, 0.8f, 1.0f, 1.0f}; // Sharp icy highlights
Color light3Color = {0.4f, 0.6f, 1.0f};

// Collection of lights for easy looping
const GLfloat *lightsAmbient[] = {light1Ambient, light2Ambient, light3Ambient};
const GLfloat *lightsDiffuse[] = {light1Diffuse, light2Diffuse, light3Diffuse};
const GLfloat *lightsSpecular[] = {light1Specular, light2Specular, light3Specular};
const Color *lightsColor[] = {&light1Color, &light2Color, &light3Color};

const int NUM_LIGHTS = sizeof(lightsAmbient) / sizeof(lightsAmbient[0]);
int lightIndex = 0;

// -------------------
// Material Setup
// -------------------
GLfloat defaultAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat defaultDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat defaultSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat defaultShininess = 0.0f;

// Gold Plate
GLfloat gold_matAmbient[] = {0.247f, 0.199f, 0.074f, 1.0f};
GLfloat gold_matDiffuse[] = {0.751f, 0.606f, 0.226f, 1.0f};
GLfloat gold_matSpecular[] = {0.628f, 0.556f, 0.366f, 1.0f};
GLfloat gold_shininess = 51.2f;

// Gray Silk Scarf
GLfloat silk_matAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
GLfloat silk_matDiffuse[] = {0.25f, 0.25f, 0.25f, 1.0f};
GLfloat silk_matSpecular[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat silk_shininess = 90.0f;

// -------------------
// GLU Quadric Objects
// -------------------
GLUquadricObj *quadric = gluNewQuadric();
GLUquadric *skyQuadric = gluNewQuadric();

// -------------------
// Textures Setup
// -------------------
// COMMON TEXTURES
GLuint brickTexture;
GLuint steelTexture;
GLuint woodTexture;
GLuint boxTexture;
GLuint goldTexture;
GLuint silverTexture;

// CHARACTER TEXTURES
GLuint skinTexture;
GLuint lipTexture;
GLuint eyelashTexture;
GLuint scleraTexture;
GLuint pupilTexture;
GLuint hairTexture;
GLuint grassTexture;

// OUTFITS TEXTURES
GLuint redBlackFlameTexture;
GLuint redClothTexture;
GLuint brownClothTexture;

// PROPS TEXTURES
GLuint beltTexture;

// Environment Textures
GLuint skyTexture;
GLuint seaTexture;


// Weapon Textures
GLuint goldenTexture;
GLuint sliverTexture;
GLuint spearBlade;
GLuint spearRedBlade;
// ---------
// Constants
// ---------
const int SLICES = 50;
const int STACKS = 50;
const int LOOPS = 50;

const float PI = 3.14159265358979f;

// ---------
const float GLU_ANGLEY_OFFSET = -90.0f;

const float MIN_ANGLE = 0.0f;
const float MAX_ANGLE = 360.0f;

const int MIN_SLICES = 10;
const int MAX_SLICES = 50;
const int MIN_STACKS = 10;
const int MAX_STACKS = 50;

const float WORLD_TOP = 1.0f;
const float WORLD_LEFT = -1.0f;
const float WORLD_BOTTOM = -1.0f;
const float WORLD_RIGHT = 1.0f;

const float GRAVITY = 0.01f;

bool InitPixelFormat(HDC hdc);
void DrawVest(float torsoRadius, float torsoHeight);

void ResetCameraPosition()
{
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;
}

void ResetCameraAngle()
{
	cameraAngleX = 0.0f;
	cameraAngleY = 0.0f;
	cameraAngleZ = 0.0f;
}

void ResetLightPosition()
{
	lightX = 0.0f;
	lightY = 0.0f;
	lightZ = 0.0f;
}

void ToggleLight()
{
	isLightOn = !isLightOn;
}

void ResetToggle()
{
	isCameraMode = false;
	isLightMode = false;
}

void ResetModel()
{
	for (int i = 0; i < PART_COUNT; i++)
	{
		parts[i].angleX = parts[i].defaultX;
		parts[i].angleY = parts[i].defaultY;
		parts[i].angleZ = parts[i].defaultZ;
	}
}

void SetTexture(GLuint tex)
{
	currentTexture = tex;
}

void UpdateCurrentTexture()
{
	if (!textures.empty())
	{
		currentTexture = textures[currentTextureIndex];
	}
}

void ApplyCharacterTexturePreset()
{
	if (characterTexturePresets.empty())
		return;

	const CharacterTexturePreset &preset = characterTexturePresets[currentCharacterTexturePresetIndex];
	skinTexture = preset.skin;
	hairTexture = preset.hair;
	redBlackFlameTexture = preset.outfitPrimary;
	brownClothTexture = preset.outfitSecondary;
	goldTexture = preset.accessory;
}

void InitializeCharacterTexturePresets()
{
	characterTexturePresets.clear();

	// Five Elements order: Fire, Water, Wood, Metal, Earth
	characterTexturePresets.push_back({skinTexture, hairTexture, redBlackFlameTexture, redClothTexture, goldTexture});
	characterTexturePresets.push_back({skinTexture, seaTexture, skyTexture, skyTexture, silverTexture});
	characterTexturePresets.push_back({skinTexture, grassTexture, woodTexture, grassTexture, goldTexture});
	characterTexturePresets.push_back({skinTexture, goldTexture, goldTexture, goldTexture, goldTexture});
	characterTexturePresets.push_back({skinTexture, brownClothTexture, brickTexture, brownClothTexture, goldTexture});

	currentCharacterTexturePresetIndex = FIRE_ELEMENT;
	ApplyCharacterTexturePreset();
}

Color GetElementClothTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return {0.15f, 0.28f, 0.42f};
	case WOOD_ELEMENT:
		return {0.35f, 0.75f, 0.32f};
	case METAL_ELEMENT:
		return {0.95f, 0.82f, 0.28f};
	case EARTH_ELEMENT:
		return {0.55f, 0.36f, 0.20f};
	default:
		return {1.0f, 1.0f, 1.0f};
	}
}

Color GetElementAccentTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return {0.86f, 0.72f, 0.36f};
	case WOOD_ELEMENT:
		return {0.55f, 0.88f, 0.45f};
	case METAL_ELEMENT:
		return {1.0f, 0.86f, 0.35f};
	case EARTH_ELEMENT:
		return {0.62f, 0.42f, 0.24f};
	default:
		return {1.0f, 1.0f, 1.0f};
	}
}

Color GetElementHairTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return {0.20f, 0.42f, 0.60f};
	default:
		return {1.0f, 1.0f, 1.0f};
	}
}

void ApplyTint(const Color &color)
{
	glColor3f(color.r, color.g, color.b);
}

void ResetTint()
{
	glColor3f(1.0f, 1.0f, 1.0f);
}

float Clamp(float v, float minV, float maxV)
{
	return max(minV, min(maxV, v));
}
// -------------------------------------------------------
//---------
//Animation
//---------
void UpdateAnimation() {
	// Only run this if we are in Animation Mode and it is playing
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	animFrame += (1.0f * animSpeed);
	float maxFrames = 90.0f;

	// Handle Looping logic
	if (animFrame > maxFrames) {
		if (isLooping) {
			animFrame = 0.0f; // Restart
		}
		else {
			animFrame = maxFrames; // Lock it at the last frame
			isPlaying = false;     // Auto-pause at the end
		}
	}

	float t = animFrame / maxFrames;

	// Variables to hold our target angles and positions
	float torsoY = 0.0f;
	float rArmX = 0.0f, rArmZ = 0.0f, rLowerArmX = 0.0f, rHandX = 0.0f;

	// NEW: Variables for the Left Arm to hold the spear!
	float lArmX = 0.0f, lArmZ = 0.0f, lLowerArmX = 0.0f;

	float lLegX = 0.0f, rLegX = 0.0f, lKneeX = 0.0f, rKneeX = 0.0f;
	float charZ = 0.0f, charY = 0.0f;

	if (currentWeaponAnim == 0) { // Spear Thrust Animation

		// Phase 1: Wind-up (0% to 40%) - Two-handed pose!
		if (t <= 0.4f) {
			float phaseT = t / 0.4f;
			float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

			torsoY = ease * -45.0f;

			// Right arm pulls back to the hip (matches your image)
			rArmX = ease * 30.0f;
			rArmZ = ease * 20.0f;
			rLowerArmX = ease * -100.0f;
			rHandX = ease * -30.0f;

			// Left arm raises up and bends to hold the front of the spear
			lArmX = ease * -60.0f;
			lArmZ = ease * -20.0f;
			lLowerArmX = ease * -70.0f;

			lLegX = ease * -20.0f;
			lKneeX = ease * 20.0f;
			rLegX = ease * 20.0f;
			rKneeX = ease * 20.0f;

			charZ = ease * -0.05f;
			charY = ease * -0.05f;
		}
		// Phase 2: The Thrust & Lunge! (40% to 60%) - Explode forward
		else if (t <= 0.6f) {
			float phaseT = (t - 0.4f) / 0.2f;
			float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

			torsoY = -45.0f + (ease * 75.0f);

			// Right hand pushes the back of the spear forward
			rArmX = 30.0f + (ease * -120.0f);
			rArmZ = 20.0f + (ease * -20.0f);
			rLowerArmX = -100.0f + (ease * 90.0f);
			rHandX = -30.0f + (ease * -60.0f);

			// Left hand guides the front of the spear
			lArmX = -60.0f + (ease * -30.0f);
			lArmZ = -20.0f + (ease * 20.0f);
			lLowerArmX = -70.0f + (ease * 50.0f);

			lLegX = -20.0f + (ease * -30.0f);
			lKneeX = 20.0f + (ease * 30.0f);
			rLegX = 20.0f + (ease * 30.0f);
			rKneeX = 20.0f + (ease * -20.0f);

			charZ = -0.05f + (ease * 0.35f);
			charY = -0.05f + (ease * -0.05f);
		}
		// Phase 3: Hold the pose (60% to 80%) 
		else if (t <= 0.8f) {
			torsoY = 30.0f;

			rArmX = -90.0f;
			rArmZ = 0.0f;
			rLowerArmX = -10.0f;
			rHandX = -90.0f;

			lArmX = -90.0f;
			lArmZ = 0.0f;
			lLowerArmX = -20.0f;

			lLegX = -50.0f;
			lKneeX = 50.0f;
			rLegX = 50.0f;
			rKneeX = 0.0f;

			charZ = 0.30f;
			charY = -0.1f;
		}
		// Phase 4: Recovery (80% to 100%) - Step back to center
		else {
			float phaseT = (t - 0.8f) / 0.2f;
			float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

			torsoY = 30.0f - (ease * 30.0f);

			rArmX = -90.0f - (ease * -90.0f);
			rArmZ = 0.0f;
			rLowerArmX = -10.0f - (ease * -10.0f);
			rHandX = -90.0f - (ease * -90.0f);

			lArmX = -90.0f - (ease * -90.0f);
			lArmZ = 0.0f;
			lLowerArmX = -20.0f - (ease * -20.0f);

			lLegX = -50.0f - (ease * -50.0f);
			lKneeX = 50.0f - (ease * 50.0f);
			rLegX = 50.0f - (ease * 50.0f);
			rKneeX = 0.0f;

			charZ = 0.30f - (ease * 0.30f);
			charY = -0.1f - (ease * -0.1f);
		}

		// Apply the calculated angles to the actual character parts!
		parts[UPPER_TORSO].angleY = torsoY;

		parts[RIGHT_UPPER_ARM].angleX = rArmX;
		parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
		parts[RIGHT_LOWER_ARM].angleX = rLowerArmX;
		parts[RIGHT_HAND].angleX = rHandX;

		// Apply the new Left Arm angles!
		parts[LEFT_UPPER_ARM].angleX = lArmX;
		parts[LEFT_UPPER_ARM].angleZ = lArmZ;
		parts[LEFT_LOWER_ARM].angleX = lLowerArmX;

		parts[LEFT_UPPER_LEG].angleX = lLegX;
		parts[LEFT_LOWER_LEG].angleX = lKneeX;
		parts[RIGHT_UPPER_LEG].angleX = rLegX;
		parts[RIGHT_LOWER_LEG].angleX = rKneeX;

		characterZ = charZ;
		characterY = charY;
	}
}
// UINT = Unsigned integer e.g. Mouse Moved (Like email title)
// WPARAM, LPARAM = Parameters
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		SetTimer(hWnd, 1, 30, NULL); // 30ms (~33 FPS animation)
		break;

	case WM_TIMER:
		UpdateAnimation();
		scarfTime += scarfSpeed;
		InvalidateRect(hWnd, NULL, FALSE); // redraw
		break;

	case WM_KEYDOWN:
	{
		PartRotation &part = parts[currentPart];

		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case 0x30: // Press 0 - WEAPON MODE
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = WEAPON_CUSTOM;
			break;
		case 0x32: // Press 2 - ANIMATION MODE
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = ANIMATION;
			isPlaying = false; // Start paused
			animFrame = 0.0f;  // Reset to the beginning
			break;
		case 0x31: // Press 1 - Interactive Movements
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = INTERACT;
			currentPart = HEAD;
			currentPart = HEAD;
			currentPart = HEAD;

			break;
		case 0x4E: // 'N' Key
			if (currentSceneMode == WEAPON_CUSTOM) {
				wheelRotationSpeed += 0.05f; // Accelerate spin
			}
			else {
				// Your existing degrip logic
				currentGrip -= 2.0f;
				if (currentGrip < -50.0f) {
					currentGrip = -50.0f;
				}
			}
			break;

		case 0x39: // Press 9 - Character Customization
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = CHARACTER_CUSTOM;
			currentPart = HEAD;

			break;

			// DEBUG : BACK VIEW
			// case 0x39:
			//	ResetCameraPosition();
			//	ResetCameraAngle();
			//	ResetLightPosition();
			//	ResetToggle();
			//	ResetModel();
		case 0x4D: // 'M' Key
			if (currentSceneMode == WEAPON_CUSTOM) {
				wheelRotationSpeed -= 0.05f; // Decelerate / spin backward
			}
			break;
		case 'T': // 'N' Key - Degrip
			currentGrip += 2.0f; // Uncurl the fingers
			// Stop them from bending backwards
			if (currentGrip > 70.0f) {
				currentGrip = 70.0f;
			}
			break;

			//	currentSceneMode = INTERACT;
			//	currentPart = HEAD;

			//	cameraZ = -2.0f;
			//	cameraAngleY = -180.0f;
			//	lightZ = -3.0f;

			//	break;

			// DEBUG : SIDE VIEW
			// case 0x30:
			//	ResetCameraPosition();
			//	ResetCameraAngle();
			//	ResetLightPosition();
			//	ResetToggle();
			//	ResetModel();

			//	currentSceneMode = INTERACT;
			//	currentPart = HEAD;

			//	cameraX = 1.0f;
			//	cameraZ = -1.0f;
			//	cameraAngleY = 90.0f;

			//	lightX = 2.0f;
			//	lightZ = -1.0f;

			//	break;

			// ----------------------
			// CAMERA / LIGHT CONTROL
			// ----------------------
		case 0x43: // [C]
			isCameraMode = !isCameraMode;
			isLightMode = false;
			break;
		case 0x56: // [V]
			isCameraMode = false;
			isLightMode = !isLightMode;
			break;

		case VK_LEFT:
			if (isCameraMode)
				cameraAngleY -= rotationFactor;
			break;
		case VK_RIGHT:
			if (isCameraMode)
				cameraAngleY += rotationFactor;
			break;
		case VK_UP:
			if (isCameraMode)
				cameraAngleX -= rotationFactor;
			break;
		case VK_DOWN:
			if (isCameraMode)
				cameraAngleX += rotationFactor;
			break;

		case 0x57: // W
			if (isCameraMode)
			{
				cameraZ -= movementFactor;
			}
			else if (isLightMode)
			{
				lightZ -= movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleX -= partRotationFactor;
					part.angleX = Clamp(part.angleX, part.minX, part.maxX);
					break;
				}
			}
			break;
		case 0x53: // S
			if (isCameraMode)
			{
				cameraZ += movementFactor;
			}
			else if (isLightMode)
			{
				lightZ += movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleX += partRotationFactor;
					part.angleX = Clamp(part.angleX, part.minX, part.maxX);
					break;
				}
			}
			break;
		case 0x41: // A
			if (isCameraMode)
			{
				cameraX -= movementFactor;
			}
			else if (isLightMode)
			{
				lightX -= movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleY -= partRotationFactor;
					part.angleY = Clamp(part.angleY, part.minY, part.maxY);
					break;
				}
			}
			break;
		case 0x44: // D
			if (isCameraMode)
			{
				cameraX += movementFactor;
			}
			else if (isLightMode)
			{
				lightX += movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleY += partRotationFactor;
					part.angleY = Clamp(part.angleY, part.minY, part.maxY);
					break;
				}
			}
			break;
		case 0x51: // Q
			if (isCameraMode)
			{
				cameraY -= movementFactor;
			}
			else if (isLightMode)
			{
				lightY -= movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleZ -= partRotationFactor;
					part.angleZ = Clamp(part.angleZ, part.minZ, part.maxZ);
					break;
				}
			}
			break;
		case 0x45: // E
			if (isCameraMode)
			{
				cameraY += movementFactor;
			}
			else if (isLightMode)
			{
				lightY += movementFactor;
			}
			else
			{
				switch (currentSceneMode)
				{
				case INTERACT:
					part.angleZ += partRotationFactor;
					part.angleZ = Clamp(part.angleZ, part.minZ, part.maxZ);
					break;
				}
			}
			break;

		case 0x4F:	// [O]
			if (currentSceneMode == ANIMATION) {
				isPlaying = true; // Play/Continue animation
			}
			else if (isCameraMode) {
				currentCameraMode = (CameraMode)((currentCameraMode + CAMERA_COUNT - 1) % CAMERA_COUNT);
			}
			else if (isLightMode) {
				lightIndex = (lightIndex + NUM_LIGHTS - 1) % NUM_LIGHTS;
			}
			break;
		case 0x50:	// [P]
			if (currentSceneMode == ANIMATION) {
				isPlaying = false; // Pause animation
			}
			else if (isCameraMode) {
				currentCameraMode = (CameraMode)((currentCameraMode + 1) % CAMERA_COUNT);
			}
			else if (isLightMode) {
				lightIndex = (lightIndex + 1) % NUM_LIGHTS;
			}
			break;

		case 0x4C:	// [L]
			if (currentSceneMode == ANIMATION) {
				isLooping = !isLooping; // Toggle looping on/off
			}
			else if (isLightMode) {
				ToggleLight(); // Normal Light Toggle
			}
			break;
		case 0x55: // [U] - SLOW DOWN
			if (currentSceneMode == ANIMATION) {
				animSpeed -= 0.25f;
				if (animSpeed < 0.25f) animSpeed = 0.25f; // Don't let it go backwards/stop entirely
			}
			break;

		case 0x49: // [I] - SPEED UP
			if (currentSceneMode == ANIMATION) {
				animSpeed += 0.25f;
			}
			break;

		case 0x5A:	// [Z] Key
			switch (currentSceneMode)
			{
			case INTERACT:
				// Switch to Previous Part
				currentPart = (Part)((currentPart + PART_COUNT - 1) % PART_COUNT);
				break;
			case WEAPON_CUSTOM:
				// Switch to Previous Blade Texture
				currentBladeIndex = (currentBladeIndex + TOTAL_BLADE_TEXTURES - 1) % TOTAL_BLADE_TEXTURES;
				break;
			case ANIMATION:
				// Switch to Previous Animation
				currentWeaponAnim = (currentWeaponAnim + TOTAL_Weapon_ANIMS - 1) % TOTAL_Weapon_ANIMS;
				animFrame = 0.0f; // Reset timeline
				break;
			case CHARACTER_CUSTOM:
				if (!characterTexturePresets.empty())
				{
					currentCharacterTexturePresetIndex = (currentCharacterTexturePresetIndex + (int)characterTexturePresets.size() - 1) % (int)characterTexturePresets.size();
					ApplyCharacterTexturePreset();
				}
				break;
			}
			break;

		case 0x58:	// [X] Key
			switch (currentSceneMode)
			{
			case INTERACT:
				// Switch to Next Part
				currentPart = (Part)((currentPart + 1) % PART_COUNT);
				break;
			case WEAPON_CUSTOM:
				// Switch to Next Blade Texture
				currentBladeIndex = (currentBladeIndex + 1) % TOTAL_BLADE_TEXTURES;
				break;
			case ANIMATION:
				// Switch to Next Animation
				currentWeaponAnim = (currentWeaponAnim + 1) % TOTAL_Weapon_ANIMS;
				animFrame = 0.0f; // Reset timeline
				break;
			case CHARACTER_CUSTOM:
				if (!characterTexturePresets.empty())
				{
					currentCharacterTexturePresetIndex = (currentCharacterTexturePresetIndex + 1) % (int)characterTexturePresets.size();
					ApplyCharacterTexturePreset();
				}
				break;
			}
			break;

		case 0x42:	// [B] Key pressed
			switch (currentSceneMode)
			{
			case CHARACTER_CUSTOM:
				// (You can add logic here later if 'B' should do something else in Character mode)
				currentCharacterMode = (currentCharacterMode == LENGZAI_MODE) ? CHIBI_MODE : LENGZAI_MODE;
				break;
			case WEAPON_CUSTOM:
				// Cycle weapons ONLY when in Weapon Mode
				currentWeapon = (currentWeapon + 1) % TOTAL_WEAPONS;
				break;
			}
			break;

		case VK_SPACE: // To reset the scene
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();
			break;
		}

		break;
	}

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

// HDC = Handler to pull a memory (Pointer) [Handle to Display context]
// Display Context = piece of memory that stores what u want to show on screen
bool InitPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void InitParts()
{
	// HEAD
	parts[HEAD] =
		{
			0.0f, 0.0f, 0.0f, // current angles

			0.0f, 0.0f, 0.0f, // defaults

			-10.0f, 20.0f, // X min/max
			-60.0f, 60.0f, // Y min/max
			-20.0f, 20.0f  // Z min/max
		};

	// TORSO
	parts[UPPER_TORSO] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-5.0f, 20.0f,
			-15.0f, 15.0f,
			-8.0f, 8.0f};

	parts[LOWER_TORSO] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-20.0f, 5.0f,
			-15.0f, 15.0f,
			-8.0f, 8.0f};

	// LEFT ARM
	parts[LEFT_UPPER_ARM] =
		{
			0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-60.0f, 60.0f,
		-30.0f, 100.0f,
		-80.0f, 80.0f
	};

	parts[LEFT_LOWER_ARM] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-10.0f, 70.0f,
			0.0f, 130.0f,
			0.0f, 0.0f};

	parts[LEFT_HAND] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-100.0f, 10.0f,
			-30.0f, 30.0f,
			-50.0f, 80.0f};

	// RIGHT ARM
	parts[RIGHT_UPPER_ARM] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,


		-60.0f, 60.0f,
		-30.0f, 100.0f,
		-80.0f, 80.0f
	};

	parts[RIGHT_LOWER_ARM] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-10.0f, 70.0f,
			0.0f, 130.0f,
			0.0f, 0.0f};

	parts[RIGHT_HAND] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-10.0f, 100.0f,
			-30.0f, 30.0f,
			-80.0f, 50.0f};

	// LEFT LEG
	parts[LEFT_UPPER_LEG] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-50.0f, 50.0f,
			-10.0f, 20.0f,
			-10.0f, 50.0f};

	parts[LEFT_LOWER_LEG] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			0.0f, 120.0f,
			0.0f, 0.0f,
			0.0f, 0.0f};

	parts[LEFT_FOOT] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-5.0f, 50.0f,
			0.0f, 0.0f,
			-30.0f, 5.0f};

	// RIGHT LEG
	parts[RIGHT_UPPER_LEG] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-50.0f, 50.0f,
			-10.0f, 20.0f,
			-10.0f, 50.0f};

	parts[RIGHT_LOWER_LEG] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			0.0f, 120.0f,
			0.0f, 0.0f,
			0.0f, 0.0f};

	parts[RIGHT_FOOT] =
		{
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 0.0f,

			-5.0f, 50.0f,
			0.0f, 0.0f,
			-30.0f, 5.0f};
}

GLuint LoadTexture(const char *filePath)
{
	BITMAP bmp;
	HBITMAP hBMP;

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), filePath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(bmp), &bmp);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

	DeleteObject(hBMP); // Clean up the bitmap handle

	return textureID;
}

void LoadCommonTextures()
{
	brickTexture = LoadTexture("Assets/Common/Brick.bmp");
	steelTexture = LoadTexture("Assets/Common/Steel.bmp");
	woodTexture = LoadTexture("Assets/Common/Wood.bmp");
	boxTexture = LoadTexture("Assets/Common/Box.bmp");
	goldTexture = LoadTexture("Assets/Common/Gold.bmp");
	silverTexture = LoadTexture("Assets/Common/Silver.bmp");
}

void LoadCharacterTextures()
{
	skinTexture = LoadTexture("Assets/Character/Skin.bmp");
	lipTexture = LoadTexture("Assets/Character/Lip.bmp");
	eyelashTexture = LoadTexture("Assets/Character/Eyelash.bmp");
	scleraTexture = LoadTexture("Assets/Character/Sclera.bmp");
	pupilTexture = LoadTexture("Assets/Character/Pupil.bmp");
	hairTexture = LoadTexture("Assets/Character/Hair.bmp");
	grassTexture = LoadTexture("Assets/Outfits/Grass.bmp");
}

void LoadOutfitTextures()
{
	redBlackFlameTexture = LoadTexture("Assets/Outfits/RedBlackFlame.bmp");
	redClothTexture = LoadTexture("Assets/Outfits/RedCloth.bmp");
	brownClothTexture = LoadTexture("Assets/Outfits/BrownCloth.bmp");
}

void LoadPropTextures()
{
	beltTexture = LoadTexture("Assets/Props/Belt.bmp");
}

void LoadEnvironmentTextures()
{
	skyTexture = LoadTexture("Assets/Environment/Sky.bmp");
	seaTexture = LoadTexture("Assets/Environment/Sea.bmp");
}
void LoadWeaponTextures()
{
	//Spear Texture
	goldenTexture = LoadTexture("Assets/Weapon/gold.bmp");
	sliverTexture = LoadTexture("Assets/Weapon/sliver.bmp");
	spearBlade = LoadTexture("Assets/Weapon/SpearBlade.bmp");
	spearRedBlade = LoadTexture("Assets/Weapon/goldenRed.bmp");
	//Fish Texture
	//Fire Wheel Texture
}

void InitTextures()
{
	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4 byte alignment = r g b a (32 bits = 4 bytes)

	LoadCommonTextures();
	LoadCharacterTextures();
	LoadOutfitTextures();
	LoadPropTextures();
	LoadEnvironmentTextures();
	InitializeCharacterTexturePresets();

	LoadWeaponTextures();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);
}

// ************
//	2D SHAPES
// ************

float DegreeToRadian(float degree)
{
	return (3.142f / 180.0f) * degree;
}

float RadianToDegree(float radian)
{
	return (180.0f / 3.142f) * radian;
}

void SetFaceNormal(
	float ax, float ay, float az,
	float bx, float by, float bz,
	float cx, float cy, float cz)
{
	float ux = bx - ax;
	float uy = by - ay;
	float uz = bz - az;

	float vx = cx - ax;
	float vy = cy - ay;
	float vz = cz - az;

	// Cross Product of U x V = a vector perpendicular to both U and V
	float nx = uy * vz - uz * vy;
	float ny = uz * vx - ux * vz;
	float nz = ux * vy - uy * vx;

	// normalize (Pythagoras)
	float length = sqrt(nx * nx + ny * ny + nz * nz);
	if (length != 0.0f)
	{
		nx /= length;
		ny /= length;
		nz /= length;
	}

	glNormal3f(nx, ny, nz);
}

void DrawRectangle(float width, float height)
{
	// To center the rectangle at the center origin
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glVertex3f(-w, h, z);
	glVertex3f(-w, -h, z);
	glVertex3f(w, -h, z);
	glVertex3f(w, h, z);
}

void DrawEquilateralTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glTexCoord2f(0.0f, 0.5f);
	glVertex3f(0.0f, h, z); // top edge
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, z); // left edge
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, z); // right edge
}

void DrawRightTriangleWithColor(float width, float height, Color c1, Color c2, Color c3)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glColor3f(c1.r, c1.g, c1.b);
	glVertex3f(-w, h, z); // top edge
	glColor3f(c2.r, c2.g, c2.b);
	glVertex3f(-w, -h, z); // left edge
	glColor3f(c3.r, c3.g, c3.b);
	glVertex3f(w, -h, z); // right edge
}

void DrawRightTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, z); // top edge
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, z); // left edge
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, z); // right edge
}

void DrawIsocelesTriangleWithColor_RightTriangle(float width, float height, Color c1, Color c2, Color c3)
{
	float w = width / 2.0f;

	glPushMatrix();
	glTranslatef(w, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	DrawRightTriangleWithColor(width, height, c1, c2, c3);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-w, 0.0f, 0.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	DrawRightTriangleWithColor(width, height, c1, c2, c3);
	glEnd();
	glPopMatrix();
}

void DrawDiamondWithColor_IsocelesTriangle(float width, float height, Color c1, Color c2, Color c3)
{
	float h = height / 2.0f;

	glPushMatrix();
	glTranslatef(0.0f, h, 0.0f);
	DrawIsocelesTriangleWithColor_RightTriangle(width, height, c1, c2, c3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -h, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	DrawIsocelesTriangleWithColor_RightTriangle(width, height, c1, c2, c3);
	glPopMatrix();
}

void DrawIsocelesTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glVertex3f(0.0f, h, z); // top edge
	glVertex3f(-w, -h, z);	// left edge
	glVertex3f(w, -h, z);	// right edge
}

void DrawCircle(float radius, int angle)
{
	float z = 0.0f;

	for (int i = 0; i < angle; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawSemiCircle(float radius)
{
	float z = 0.0f;

	// glVertex3f(centerX, centerY, z);

	for (int i = 0; i <= 180; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawArc(float radius, float angle)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	glVertex3f(x, y, z);

	for (int i = 0; i <= angle; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawArcWithCurveOnly(float radius, float angle)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	glBegin(GL_LINE_STRIP);

	for (int i = 0; i <= angle; i += 1)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}

	glEnd();
}

void DrawPentagon(float radius)
{
	float z = 0.0f;

	if (radius > 1)
		return;
	for (int i = 0; i < 5; i++)
	{
		float angle = 72.0f * i + 90.0f; // 360 / 5 = 72 (each angle is 72deg)
		float tempX = cos(DegreeToRadian(angle)) * radius;
		float tempY = sin(DegreeToRadian(angle)) * radius;

		glVertex3f(tempX, tempY, z);
	}
}

void DrawLineWithColor(float length, float width, Color c)
{
	float z = 0.0f;

	glColor3f(c.r, c.g, c.b);
	glLineWidth(width);
	glBegin(GL_LINES);
	glVertex3f(-length / 2.0f, 0.0f, z);
	glVertex3f(length / 2.0f, 0.0f, z);
	glEnd();
}

// ***********
//	3D Shapes
// ***********

void DrawCuboidPolygon(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// FRONT
	glNormal3f(0, 0, 1);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, h, d);
	glEnd();

	// RIGHT
	glNormal3f(1, 0, 0);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(w, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, h, -d);
	glEnd();

	// BACK
	glNormal3f(0, 0, -1);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glEnd();

	// LEFT
	glNormal3f(-1, 0, 0);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, d);
	glEnd();

	// TOP
	glNormal3f(0, 1, 0);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, h, -d);
	glEnd();

	// BOTTOM
	glNormal3f(0, -1, 0);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
}

void DrawCuboidLines(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, d);
	glVertex3f(-w, -h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, h, d);
	glEnd();

	// Right face
	glBegin(GL_LINE_LOOP);
	glVertex3f(w, h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, -h, -d);
	glVertex3f(w, h, -d);
	glEnd();

	// Back face
	glBegin(GL_LINE_LOOP);
	glVertex3f(w, h, -d);
	glVertex3f(w, -h, -d);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, h, -d);
	glEnd();

	// Left face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, -d);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, -h, d);
	glVertex3f(-w, h, d);
	glEnd();

	// Top face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, -d);
	glVertex3f(-w, h, d);
	glVertex3f(w, h, d);
	glVertex3f(w, h, -d);
	glEnd();

	// Bottom face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, -h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, -h, -d);
	glEnd();
}

void DrawPrism(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, d);
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, d,
		-w, -h, d,
		w, -h, d);
	DrawEquilateralTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f, // top front right
		w, -h, d,	// bottom front right
		w, -h, -d	// bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -d);
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, -d,
		-w, -h, -d,
		w, -h, -d);
	DrawEquilateralTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-w, h, 0.0f, // top front right
		-w, -h, -d,	 // bottom front right
		-w, -h, d	 // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, h, d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawPrismWithOffset(float width, float height, float depth, float topOffset)
{
	if (topOffset > width)
		return;

	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	float offsetZ = (depth - topOffset) / 2.0f;

	// Front face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, d - offsetZ,
		-w, -h, d,
		w, -h, d);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, d - offsetZ);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f, // top front right
		w, -h, d,	// bottom front right
		w, -h, -d	// bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, h, d - offsetZ);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, h, -(d - offsetZ));
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, -(d - offsetZ),
		w, -h, -d,
		-w, -h, -d);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, -(d - offsetZ));
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-w, h, 0.0f, // top front right
		-w, -h, -d,	 // bottom front right
		-w, -h, d	 // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, h, -(d - offsetZ));
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, h, d - offsetZ);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawRightTriangularPrism(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glTranslatef(0.0f, 0.0f, d);
	SetFaceNormal(
		0.0f, h, d,
		-w, -h, d,
		w, -h, d);
	DrawRightTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Right / Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f, // top front right
		w, -h, d,	// bottom front right
		w, -h, -d	// bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glTranslatef(0.0f, 0.0f, -d);
	SetFaceNormal(
		0.0f, h, -d,
		w, -h, -d,
		-w, -h, -d);
	DrawRightTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawTrapezoidalPrism(float width, float height, float depth, float topOffset)
{
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	float t = topOffset / 2;

	// Front face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-t, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		t, h, 0.0f, // top front right
		w, -h, d,	// bottom front right
		w, -h, -d	// bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(t, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(t, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-t, h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-t, h, 0.0f, // top front right
		-w, -h, -d,	 // bottom front right
		-w, -h, d	 // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-t, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-t, h, d);
	glEnd();
	glPopMatrix();

	// Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-t, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-t, h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(t, h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawRightTrapezoidalPrism(float width, float height, float depth, float topOffset)
{
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	float t = topOffset / 2;

	// Front face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		t, h, 0.0f, // top front right
		w, -h, d,	// bottom front right
		w, -h, -d	// bottom back right
	);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(t, h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(t, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-w, h, d);
	glEnd();
	glPopMatrix();

	// Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, h, d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(t, h, d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawPyramid(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;

	float apexX = 0.0f, apexY = h, apexZ = 0.0f;

	// Front face
	SetFaceNormal(apexX, apexY, apexZ, -w, -h, w, w, -h, w);
	glBegin(GL_TRIANGLES);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, w);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, w);
	glEnd();

	// Right face
	SetFaceNormal(apexX, apexY, apexZ, w, -h, w, w, -h, -w);
	glBegin(GL_TRIANGLES);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, w);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, -w);
	glEnd();

	// Back face
	SetFaceNormal(apexX, apexY, apexZ, w, -h, -w, -w, -h, -w);
	glBegin(GL_TRIANGLES);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(w, -h, -w);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, -w);
	glEnd();

	// Left face
	SetFaceNormal(apexX, apexY, apexZ, -w, -h, -w, -w, -h, w);
	glBegin(GL_TRIANGLES);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, -w);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-w, -h, w);
	glEnd();

	glEnd();

	// Bottom face
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-w, -h, -w);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, -h, w);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(w, -h, w);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(w, -h, -w);
	glEnd();
}

void DrawDisk(GLUquadricObj *disk, float innerRadius, float outerRadius, int slices, int loops)
{
	glPushMatrix();
	glRotatef(-GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluDisk(disk, innerRadius, outerRadius, slices, loops);
	glPopMatrix();
}

void DrawFlatCircle(GLUquadricObj *disk, float radius, int slices, int loops)
{
	DrawDisk(disk, 0.0f, radius, slices, loops);
}

void DrawCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	glRotatef(GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);
	glPopMatrix();
}

void DrawSemiCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = {1.0, 0.0, 0.0, 0.0};
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);
}

void DrawEnclosedCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	// Bottom Surface
	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawFlatCircle(quadric, baseRadius, slices, stacks);
	glPopMatrix();
	// END Bottom Surface

	// Top Surface
	glPushMatrix();
	glTranslatef(0.0f, offsetY, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawFlatCircle(quadric, topRadius, slices, stacks);
	glPopMatrix();
	// END Top Surface

	glPopMatrix();
}

void DrawPartialEnclosedCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	if (isBottomClosed)
	{
		// Bottom Surface
		glPushMatrix();
		glTranslatef(0.0f, -offsetY, 0.0f);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		DrawFlatCircle(quadric, baseRadius, slices, stacks);
		glPopMatrix();
		// END Bottom Surface
	}

	if (isTopClosed)
	{
		// Top Surface
		glPushMatrix();
		glTranslatef(0.0f, offsetY, 0.0f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		DrawFlatCircle(quadric, topRadius, slices, stacks);
		glPopMatrix();
		// END Top Surface
	}

	glPopMatrix();
}

void DrawEnclosedSemiCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = {1.0, 0.0, 0.0, 0.0};
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);
}

void DrawPartialEnclosedSemiCylinder(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = {1.0, 0.0, 0.0, 0.0};
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawPartialEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks, isBottomClosed, isTopClosed);

	glDisable(GL_CLIP_PLANE0);
}

void DrawEnclosedCylinderWithThickness(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	float innerBottomRadius = baseRadius * thickness;
	float innerTopRadius = topRadius * thickness;

	// Bottom Surface
	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawDisk(quadric, innerBottomRadius, baseRadius, slices, loops);
	glPopMatrix();
	// END Bottom Surface

	// Top Surface
	glPushMatrix();
	glTranslatef(0.0f, offsetY, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawDisk(quadric, innerTopRadius, topRadius, slices, loops);
	glPopMatrix();
	// END Top Surface

	glPopMatrix();
}

void DrawEnclosedSemiCylinderWithThickness(GLUquadricObj *cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = {1.0, 0.0, 0.0, 0.0};
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinderWithThickness(cylinder, baseRadius, topRadius, height, thickness, slices, stacks, loops);

	glDisable(GL_CLIP_PLANE0);
}

void DrawBentCylinder(GLUquadricObj *cylinder, float tubeRadius, float edgeRadius, float arcRadius, float bendAngle, int segments, float slices, float stacks)
{
	float step = bendAngle / segments;

	glPushMatrix();

	for (int i = 0; i < segments; i++)
	{
		float currentAngle = i * step;

		// Position on arc (XY plane)
		float x = cos(DegreeToRadian(currentAngle)) * arcRadius;
		float y = sin(DegreeToRadian(currentAngle)) * arcRadius;

		// Move to arc position
		glPushMatrix();
		glTranslatef(x, y, 0.0f);

		// Rotate so cylinder follows tangent
		// Tangent angle = currentAngle + 90
		glRotatef(currentAngle + 90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

		// Draw small cylinder segment
		if (i == 0)
		{
			gluCylinder(cylinder, tubeRadius, edgeRadius, arcRadius * DegreeToRadian(step), slices, stacks);
		}
		else if (i == segments - 1)
		{
			gluCylinder(cylinder, edgeRadius, tubeRadius, arcRadius * DegreeToRadian(step), slices, stacks);
		}
		else
		{
			gluCylinder(cylinder, tubeRadius, tubeRadius, arcRadius * DegreeToRadian(step), slices, stacks);
		}

		glPopMatrix();
	}

	glPopMatrix();
}

void DrawEnclosedBentCylinder(GLUquadricObj *cylinder, float tubeRadius, float edgeRadius, float arcRadius, float bendAngle, int segments, float slices, float stacks)
{
	float step = bendAngle / segments;
	float segmentLength = arcRadius * DegreeToRadian(step);

	glPushMatrix();

	for (int i = 0; i < segments; i++)
	{
		float currentAngle = i * step;

		float x = cos(DegreeToRadian(currentAngle)) * arcRadius;
		float y = sin(DegreeToRadian(currentAngle)) * arcRadius;

		// Segmented Cylinder
		glPushMatrix();

		glTranslatef(x, y, 0.0f);

		glRotatef(currentAngle + 90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

		if (i == 0)
			gluCylinder(cylinder, tubeRadius, edgeRadius, segmentLength, slices, stacks);
		else if (i == segments - 1)
			gluCylinder(cylinder, edgeRadius, tubeRadius, segmentLength, slices, stacks);
		else
			gluCylinder(cylinder, tubeRadius, tubeRadius, segmentLength, slices, stacks);

		// Start Cap
		if (i == 0)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, segmentLength);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricDrawStyle(cylinder, GLU_FILL);
			DrawFlatCircle(cylinder, edgeRadius, slices, stacks);
			glPopMatrix();
		}
		// END Start Cap

		// End Cap
		if (i == segments - 1)
		{
			glPushMatrix();
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricDrawStyle(cylinder, GLU_FILL);
			DrawFlatCircle(cylinder, edgeRadius, slices, stacks);
			glPopMatrix();
		}
		// END End Cap

		glPopMatrix();
		// END Segmented Cylinder
	}

	glPopMatrix();
}

void DrawSphere(GLUquadricObj *quadric, float radius, int slices, int stacks)
{
	glPushMatrix();
	gluSphere(quadric, radius, slices, stacks);
	glPopMatrix();
}

void DrawSemiSphere(GLUquadricObj *quadric, float radius, int slices, int stacks)
{
	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = {1.0, 0.0, 0.0, 0.0};
	// Equation: x >= 0 side is kept
	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);
	// Draw full sphere (but clipped)
	DrawSphere(quadric, radius, slices, stacks);
	glDisable(GL_CLIP_PLANE0);
}

// -----------------
// SPECIAL SHAPES
// -----------------
void DrawSineCurve(
	float length,
	float amplitude,
	float frequency,
	float phase,
	float yOffset,
	int segments)
{
	glBegin(GL_LINE_STRIP);

	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / segments;
		float x = t * length;

		float y = yOffset + amplitude * sinf(frequency * x + phase);

		// add slight depth wobble so it's not flat
		float z = amplitude * 0.5f * cosf(frequency * x * 0.5f + phase);

		glVertex3f(x, y, z);
	}

	glEnd();
}

void DrawCosineCurve(
	float length,
	float amplitude,
	float frequency,
	float phase,
	float yOffset,
	int segments)
{
	glBegin(GL_LINE_STRIP);

	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / segments;
		float x = t * length;

		float y = yOffset + amplitude * cosf(frequency * x + phase);
		float z = amplitude * 0.5f * sinf(frequency * x * 0.5f + phase);

		glVertex3f(x, y, z);
	}

	glEnd();
}

// -----------------
// PROJECTION SETUP
// -----------------

void SetPerspectiveProjection(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}

void SetOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

// -----------------
// CAMERA SETUP
// -----------------

void SetupCameraMode()
{
	switch (currentCameraMode)
	{
	case PERSPECTIVE:
		SetPerspectiveProjection(60.0f, 1.0f, 0.1f, 100.0f);
		// Camera (inverse transform)
		glRotatef(-cameraAngleX, 1.0f, 0.0f, 0.0f);
		glRotatef(-cameraAngleY, 0.0f, 1.0f, 0.0f);
		glRotatef(-cameraAngleZ, 0.0f, 0.0f, 1.0f);
		glTranslatef(-cameraX, -cameraY, -cameraZ);
		break;

	case ORTHO:
		SetOrthoProjection(-0.5f, 0.5f, -0.5f, 0.5f, 0.1f, 100.0f);
		// Camera (inverse transform)
		glRotatef(-cameraAngleX, 1.0f, 0.0f, 0.0f);
		glRotatef(-cameraAngleY, 0.0f, 1.0f, 0.0f);
		glRotatef(-cameraAngleZ, 0.0f, 0.0f, 1.0f);
		glTranslatef(-cameraX, -cameraY, -cameraZ);
		break;

	case LSIDE:
		SetPerspectiveProjection(60.0f, 1.0f, 0.1f, 100.0f);

		gluLookAt(
			-1.0f, 1.0f, 0.0f,					// cam position (left side)
			characterX, characterY, characterZ, // character position
			0.0f, 1.0f, 0.0f					// up vector
		);

		break;
	case RSIDE:
		SetPerspectiveProjection(60.0f, 1.0f, 0.1f, 100.0f);

		gluLookAt(
			1.0f, 1.0f, 0.0f,					// cam position (right side)
			characterX, characterY, characterZ, // character position
			0.0f, 1.0f, 0.0f					// up vector
		);

		break;
	}
}

void SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	SetupCameraMode();
	glMatrixMode(GL_MODELVIEW);
}
// ------------------
// Weapon
// ------------------

void DrawSpear(float scale) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL); // Ensure quadrics are solid

	// Enable textures for the quadric shapes!
	gluQuadricTexture(quad, GL_TRUE);

	glPushMatrix();

	// Apply the global scale here!
	glScalef(scale, scale, scale);
	glRotatef(-90, 1, 0, 0);

	// Enable 2D Texturing and set base color to white
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	// ===== 1. BOTTOM SCREW & SPIRAL (GOLD) =====
	glBindTexture(GL_TEXTURE_2D, goldenTexture);

	glPushMatrix();
	glTranslatef(0, 0, -0.6f);
	gluCylinder(quad, 0.04, 0.04, 0.6, 15, 1);
	glPopMatrix();

	float totalHeight = 0.65f;
	int segments = 150;
	float totalTurns = 6.0f;
	float shaftR = 0.045f;
	float thickness = 0.1f;

	glPushMatrix();
	glTranslatef(0, 0, -0.55f);
	for (int side = 0; side < 2; side++) {
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= segments; i++) {
			float t = (float)i / segments;
			float outerR = 0.18f - (t * 0.10f);
			float zBase = pow(t, 1.8f) * totalHeight;
			float angle = t * totalTurns * 2.0f * 3.14159f;
			float zPos = (side == 0) ? zBase : zBase - thickness;

			// Map the texture along the spiral
			glTexCoord2f(t * totalTurns, 0.0f);
			glVertex3f(cos(angle) * outerR, sin(angle) * outerR, zPos);

			glTexCoord2f(t * totalTurns, 1.0f);
			glVertex3f(cos(angle) * shaftR, sin(angle) * shaftR, zPos);
		}
		glEnd();
	}
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; i++) {
		float t = (float)i / segments;
		float outerR = 0.18f - (t * 0.10f);
		float zBase = pow(t, 1.8f) * totalHeight;
		float angle = t * totalTurns * 2.0f * 3.14159f;

		glTexCoord2f(t * totalTurns, 0.0f);
		glVertex3f(cos(angle) * outerR, sin(angle) * outerR, zBase);

		glTexCoord2f(t * totalTurns, 1.0f);
		glVertex3f(cos(angle) * outerR, sin(angle) * outerR, zBase - thickness);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -0.75f);
	gluCylinder(quad, 0.0, 0.18, 0.2, 20, 1);
	glPopMatrix();

	// ===== 2. MAIN SILVER SHAFT (SILVER) =====
	glBindTexture(GL_TEXTURE_2D, sliverTexture);
	gluCylinder(quad, 0.05, 0.05, 2.5, 15, 1);

	// ===== 3. LEAF DECORATION (GOLD) =====
	glBindTexture(GL_TEXTURE_2D, goldenTexture);
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 6; i++) {
			glPushMatrix();
			glTranslatef(0, 0, 2.5f + (j * 0.1f));
			glRotatef(i * 60 + (j * 30), 0, 0, 1);
			glRotatef(25, 1, 0, 0);
			glScalef(0.6, 1.3, 0.1);
			gluSphere(quad, 0.18, 12, 12);
			glPopMatrix();
		}
	}

	// ===== 4. THE BLADE (SPEAR BLADE) =====
	// Check which texture is currently selected
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, spearBlade);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, spearRedBlade);
	}
	glPushMatrix();
	glTranslatef(0, 0, 2.65f);

	glPushMatrix();
	glScalef(1.5, 0.4, 0.7);
	gluSphere(quad, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 0.05f);
	glScalef(2.2, 0.25, 1.0);
	gluCylinder(quad, 0.12, 0.0, 1.2, 20, 1);
	glPopMatrix();

	glPopMatrix();

	// Turn textures back off so it doesn't affect other untextured models
	glDisable(GL_TEXTURE_2D);

	glPopMatrix(); // End of global scale
	gluDeleteQuadric(quad);
}

void DrawWindFireWheel(float size) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL); // Always solid

	glPushMatrix();
	glScalef(size, size, size);

	// ==========================================
	// NEW: ANIMATION LOGIC
	// ==========================================
	wheelRotationAngle += wheelRotationSpeed;

	// Keep the angle within a valid 360-degree range
	if (wheelRotationAngle >= 360.0f) wheelRotationAngle -= 360.0f;
	if (wheelRotationAngle <= -360.0f) wheelRotationAngle += 360.0f;

	// Spin the wheel around the Z axis
	glRotatef(wheelRotationAngle, 0.0f, 0.0f, 1.0f);
	// ==========================================

	// --- 1. THE MAIN RING (Bronze) ---
	glColor3f(0.8f, 0.5f, 0.2f);
	glPushMatrix();
	glTranslatef(0, 0, -0.05f);
	gluCylinder(quad, 0.7, 0.7, 0.1, 60, 1);
	gluCylinder(quad, 0.8, 0.8, 0.1, 60, 1);
	gluDisk(quad, 0.7, 0.8, 60, 1);
	glTranslatef(0, 0, 0.1f);
	gluDisk(quad, 0.7, 0.8, 60, 1);
	glPopMatrix();

	// --- 2. THE SMOOTH 3D FLAME BLADES ---
	float thickness = 0.06f;
	const int resolution = 30;

	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glRotatef(i * 60.0f, 0, 0, 1);
		glTranslatef(0.78f, 0, 0);

		float outerX[resolution], outerY[resolution];
		float innerX[resolution], innerY[resolution];

		for (int j = 0; j < resolution; j++) {
			float t = (float)j / (resolution - 1);
			outerX[j] = t * 0.8f;
			outerY[j] = sin(t * 3.14159f) * 0.4f + (t * t * 0.3f);
			innerX[j] = t * 0.6f;
			innerY[j] = (t < 0.5f) ? (t * 0.2f) : (0.1f + (t - 0.5f) * 0.8f);
			innerY[j] -= 0.1f;
		}

		// A. OUTER GOLDEN FRAME
		glColor3f(1.0f, 0.8f, 0.3f);

		// Front Face
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < resolution; j++) {
			glVertex3f(innerX[j], innerY[j], thickness / 2);
			glVertex3f(outerX[j], outerY[j], thickness / 2);
		}
		glEnd();

		// Back Face
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < resolution; j++) {
			glVertex3f(innerX[j], innerY[j], -thickness / 2);
			glVertex3f(outerX[j], outerY[j], -thickness / 2);
		}
		glEnd();

		// Connecting Edges (The "Thickness")
		glColor3f(0.7f, 0.5f, 0.1f);
		glBegin(GL_QUAD_STRIP); // Outer rim
		for (int j = 0; j < resolution; j++) {
			glVertex3f(outerX[j], outerY[j], thickness / 2);
			glVertex3f(outerX[j], outerY[j], -thickness / 2);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP); // Inner rim
		for (int j = 0; j < resolution; j++) {
			glVertex3f(innerX[j], innerY[j], thickness / 2);
			glVertex3f(innerX[j], innerY[j], -thickness / 2);
		}
		glEnd();

		// B. THE RED CORE GLOW
		glColor3f(0.9f, 0.1f, 0.0f);
		glPushMatrix();
		glTranslatef(0.15f, 0.1f, 0.0f);
		glScalef(1.5f, 0.6f, 1.5f);
		gluSphere(quad, 0.18, 20, 20);
		glPopMatrix();

		glPopMatrix();
	}

	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawFishSword(float scale) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL); // Always solid

	glPushMatrix();
	glScalef(scale, scale, scale);

	// --- 1. Main Fish Body (Light Blue Base) ---
	glColor3f(0.7f, 0.85f, 0.95f);
	glPushMatrix();
	glScalef(0.12f, 1.5f, 0.35f);
	gluSphere(quad, 1.0, 30, 30);
	glPopMatrix();

	// --- 2. Upper Back (Darker Blue Top) ---
	glColor3f(0.2f, 0.4f, 0.7f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.08f);
	glScalef(0.11f, 1.45f, 0.3f);
	gluSphere(quad, 1.0, 20, 20);
	glPopMatrix();

	// --- 3. Long Tail Stalk (The Handle!) ---
	glColor3f(0.2f, 0.4f, 0.7f);
	glPushMatrix();
	glTranslatef(0.0f, 1.4f, 0.0f);
	glScalef(0.04f, 0.7f, 0.08f);
	gluSphere(quad, 1.0, 20, 20);
	glPopMatrix();

	// --- 4. Eyes ---
	// Right eye
	glColor3f(0.8f, 0.1f, 0.1f);
	glPushMatrix();
	glTranslatef(0.07f, -1.25f, 0.05f);
	gluSphere(quad, 0.05, 10, 10);
	glColor3f(1.0f, 0.8f, 0.0f); // Yellow ring
	glRotatef(90, 0, 1, 0);
	gluCylinder(quad, 0.06, 0.06, 0.02, 10, 1);
	glPopMatrix();

	// Left eye
	glColor3f(0.8f, 0.1f, 0.1f);
	glPushMatrix();
	glTranslatef(-0.07f, -1.25f, 0.05f);
	gluSphere(quad, 0.05, 10, 10);
	glColor3f(1.0f, 0.8f, 0.0f);
	glRotatef(-90, 0, 1, 0);
	gluCylinder(quad, 0.06, 0.06, 0.02, 10, 1);
	glPopMatrix();

	// --- 5. Tail Fin ---
	glColor3f(0.2f, 0.4f, 0.7f);
	glPushMatrix();
	glTranslatef(0.0f, 1.9f, 0.0f);
	glBegin(GL_TRIANGLES);
	// Top Tail Fluke
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.4f);
	glVertex3f(0.0f, 0.3f, 0.0f);
	// Bottom Tail Fluke
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.3f, 0.0f);
	glVertex3f(0.0f, 0.5f, -0.4f);
	glEnd();
	glPopMatrix();

	// --- 6. Dorsal Fin (Top/Back) ---
	glColor3f(0.2f, 0.4f, 0.7f);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, -0.6f, 0.25f);
	glVertex3f(0.0f, 0.6f, 0.25f);
	glVertex3f(0.0f, 0.1f, 0.5f);
	glEnd();
	glPopMatrix();

	// --- 7. Pectoral Fins (The "Hands") ---
	// Right Fin (+X)
	glColor3f(0.7f, 0.85f, 0.95f);
	glPushMatrix();
	glTranslatef(0.11f, -0.8f, -0.05f);
	glRotatef(-15, 0, 0, 1);
	glRotatef(-180, 1, 0, 0);
	glScalef(0.02f, 0.3f, 0.15f);
	glTranslatef(0.0f, -1.0f, 0.0f); // Pivot point fix
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	// Left Fin (-X)
	glPushMatrix();
	glTranslatef(-0.11f, -0.8f, -0.05f);
	glRotatef(15, 0, 0, 1);
	glRotatef(-180, 1, 0, 0);
	glScalef(0.02f, 0.3f, 0.15f);
	glTranslatef(0.0f, -1.0f, 0.0f); // Pivot point fix
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	glPopMatrix();
	gluDeleteQuadric(quad);
}
// -----------------
// LIGHTING SETUP
// -----------------
void SetupLighting()
{
	if (isLightOn)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT0, GL_AMBIENT, lightsAmbient[lightIndex]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightsDiffuse[lightIndex]);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightsSpecular[lightIndex]);

		GLfloat lightPosition[] = {lightX, lightY, lightZ, 1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	}
	else
	{
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}
}

void ResetMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);
}

void DrawLightIndicator()
{
	// DEBUG [LIGHT VISUALIZER]
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	if (isLightOn)
	{

		// Color lightVisualizerColor = { 1.0f, 1.0f, 0.0f };
		const Color lightVisualizerColor =
			{
				lightsColor[lightIndex]->r,
				lightsColor[lightIndex]->g,
				lightsColor[lightIndex]->b};

		glPushMatrix();
		glTranslatef(lightX, lightY, lightZ);
		glColor3f(lightVisualizerColor.r, lightVisualizerColor.g, lightVisualizerColor.b);
		DrawSphere(quadric, 0.03f, SLICES, STACKS);
		glPopMatrix();
	}

	glEnable(GL_LIGHTING);
	// END DEBUG [LIGHT VISUALIZER]
}

// -----------------------------------------------------------------
// *********************** UTILITY FUNCTIONS ***********************
// -----------------------------------------------------------------

// ***********************
// PROPS FUNCTIONS
// ***********************

void DrawNeckRing(float neckRadius)
{
	float ringRadius = neckRadius * 2.0f;
	float ringHeight = 0.01f;
	float thickness = 0.8f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);

	glPushMatrix();
	glRotatef(5.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, ringRadius, ringRadius, ringHeight, thickness, SLICES, STACKS, LOOPS);
	glPopMatrix();

	// float ringRadius = neckRadius * 0.3f;
	// float ringArcRadius = neckRadius * 2.0f;
	// float ringBendAngle = 360.0f;
	// int ringSegments = 50;

	// glPushMatrix();
	// glRotatef(95.0f, 1.0f, 0.0f, 0.0f);
	// DrawEnclosedBentCylinder(quadric, ringRadius, ringRadius, ringArcRadius, ringBendAngle, ringSegments, SLICES, STACKS);
	// glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void DrawEarRing(float side)
{
	float ringRadius = 0.02f;
	float ringHeight = 0.01f;
	float ringThickness = 0.7f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);

	glPushMatrix();
	glTranslatef(0.0f, -ringRadius, 0.0f);
	glRotatef(-side * 50.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinderWithThickness(quadric, ringRadius, ringRadius, ringHeight, ringThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void DrawHairRibbon(float headBaseRadius)
{
	float ribbonBaseRadius = 0.03f;
	float ribbonBaseHeight = 0.015f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, redClothTexture);

	// Center Cylinder
	glPushMatrix();
	glTranslatef(-headBaseRadius * 1.0f, -headBaseRadius * 0.45f, 0.0f);
	glRotatef(48.0f, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 1.0f, 0.4f);
	DrawCylinder(quadric, ribbonBaseRadius, ribbonBaseRadius, ribbonBaseHeight, SLICES, STACKS);

	glRotatef(-100.0f, 0.0f, 1.0f, 0.0f);

	float stripRadius = ribbonBaseRadius * 1.5f;
	float stripHeight = ribbonBaseHeight;

	// Front Strip
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, stripRadius);
	glScalef(1.0f, 1.0f, 0.8f);
	DrawSemiCylinder(quadric, stripRadius, stripRadius, stripHeight, SLICES, STACKS);
	glPopMatrix();
	// END Front Strip

	// Back Strip
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, stripRadius);
	glScalef(-1.0f, 1.0f, 1.4f);
	DrawSemiCylinder(quadric, stripRadius, stripRadius, stripHeight, SLICES, STACKS);
	glPopMatrix();
	// END Back Strip

	glPopMatrix();
	// END Center Cylinder

	glDisable(GL_TEXTURE_2D);
}

void DrawHairRibbons(float headBaseRadius)
{
	// Left Ribbon
	glPushMatrix();
	glScalef(1.0f, 1.0f, 1.0f);
	DrawHairRibbon(headBaseRadius);
	glPopMatrix();
	// END Left Ribbon

	// Right Ribbon
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);
	DrawHairRibbon(headBaseRadius);
	glPopMatrix();
	// END Right Ribbon
}

void DrawVest(float torsoRadius, float torsoHeight)
{
	float vestRadius = torsoRadius * 1.2f;
	float vestHeight = torsoRadius * 1.8f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, redBlackFlameTexture);

	// Left Side Vest
	glPushMatrix();
	glTranslatef(-torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(-3.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Left Side Top Semi Cylinder
	float topVestHeight = vestHeight * 0.2f;
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, vestRadius * 1.5f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Left Side Top Semi Cylinder

	glPopMatrix();
	// END Left Side Vest

	// Right Side Vest
	glPushMatrix();
	glTranslatef(torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Right Side Top Semi Cylinder
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, vestRadius * 1.5f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Right Side Top Semi Cylinder

	glPopMatrix();
	// END Right Side Vest

	// Back Side Vest
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -torsoRadius / 2);
	glScalef(1.0f, 1.62f, 0.9f);
	DrawCuboidPolygon(torsoRadius, vestHeight, torsoRadius);
	glPopMatrix();
	// END Back Side Vest

	glDisable(GL_TEXTURE_2D);
}

void DrawBelt(float torsoRadius)
{
	float outerBeltRadius = torsoRadius * 1.2f;
	float outerBeltHeight = 0.02f;
	float outerBeltThickness = 0.8f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, beltTexture);

	// Center Belt
	glPushMatrix();
	glScalef(1.3f, 1.0f, 0.7f);
	DrawEnclosedCylinderWithThickness(quadric, outerBeltRadius, outerBeltRadius, outerBeltHeight, outerBeltThickness, SLICES, STACKS, LOOPS);

	// Center Front Tie
	float centerTieRadius = outerBeltRadius * 0.13f;
	float centerTieHeight = outerBeltRadius * 0.2f;
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, outerBeltRadius + centerTieRadius / 2);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, centerTieRadius, centerTieRadius, centerTieHeight, SLICES, STACKS);

	// Left Front Tie
	float leftFrontTieRadius = centerTieRadius * 1.2f;
	float leftFrontTieHeight = centerTieHeight * 0.7f;
	float leftFrontTieThickness = 0.3f;
	glPushMatrix();
	glTranslatef(-leftFrontTieRadius * 0.8f, leftFrontTieRadius * 1.5f, -centerTieRadius * 0.5f);
	glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(2.0f, 1.0f, 1.2f);
	DrawEnclosedCylinderWithThickness(quadric, leftFrontTieRadius, leftFrontTieRadius, leftFrontTieHeight, leftFrontTieThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Left Front Tie

	// Right Front Tie
	float rightFrontTieRadius = centerTieRadius * 1.5f;
	float rightFrontTieHeight = centerTieHeight * 0.7f;
	float rightFrontTieThickness = 0.15f;
	glPushMatrix();
	glTranslatef(-rightFrontTieRadius * 1.8f, -rightFrontTieRadius * 1.5f, -centerTieRadius * 0.5f);
	glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(3.0f, 1.0f, 1.2f);
	DrawEnclosedCylinderWithThickness(quadric, rightFrontTieRadius, rightFrontTieRadius, rightFrontTieHeight, rightFrontTieThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Right Front Tie

	glPopMatrix();
	// END Center Front Tie

	glScalef(1.0f, 1.0f, 1.5f);

	// Left Back Tie
	float leftBackTieRadius = torsoRadius * 0.7f;
	glPushMatrix();
	glTranslatef(-leftBackTieRadius * 0.7f, -torsoRadius * 0.3f, 0.0f);
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinderWithThickness(quadric, leftBackTieRadius, leftBackTieRadius, outerBeltHeight, outerBeltThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Left Back Tie

	// Right Back Tie
	float rightBackTieRadius = torsoRadius * 0.8f;
	glPushMatrix();
	glTranslatef(rightBackTieRadius * 0.7f, -torsoRadius * 0.5f, 0.0f);
	glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinderWithThickness(quadric, leftBackTieRadius, leftBackTieRadius, outerBeltHeight, outerBeltThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Right Back Tie

	glPopMatrix();
	// END Center Belt

	glDisable(GL_TEXTURE_2D);
}

void DrawSideScarf(
	float length,
	float amplitude,
	float frequency,
	float phase,
	float width,
	float yOffset,
	int segments)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, silk_matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, silk_matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, silk_matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, silk_shininess);

	glBegin(GL_QUAD_STRIP);

	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / segments;
		float x = t * length;

		// Time-based waves (Disturbed / turbulent motion)
		float wave1 = frequency * x + phase + scarfTime;
		float wave2 = (frequency * 0.5f) * x + phase * 1.3f + scarfTime * 1.7f;

		// Falloff (root stiff, tip loose)
		float falloff = t;

		// Final position
		float y = yOffset + amplitude * falloff * (sinf(wave1) + 0.3f * sinf(wave2));
		float z = amplitude * 0.5f * falloff * (cosf(wave1 * 0.7f) + 0.3f * cosf(wave2));

		// derivative approximation (tangent)
		float dx = 0.01f;

		float wave1_2 = frequency * (x + dx) + phase + scarfTime;
		float wave2_2 = (frequency * 0.5f) * (x + dx) + phase * 1.3f + scarfTime * 1.7f;

		float y2 = yOffset + amplitude * (t + dx / length) * (sinf(wave1_2) + 0.3f * sinf(wave2_2));

		float z2 = amplitude * 0.5f * (t + dx / length) * (cosf(wave1_2 * 0.7f) + 0.3f * cosf(wave2_2));

		// Tangent
		float tx = dx;
		float ty = y2 - y;
		float tz = z2 - z;

		// Normal
		float nx = -ty;
		float ny = tx;
		float nz = 0.0f;

		float len = sqrtf(nx * nx + ny * ny + nz * nz);
		nx /= len;
		ny /= len;
		nz /= len;

		// two sides of scarf
		glVertex3f(x + nx * width, y + ny * width, z + nz * width);
		glVertex3f(x - nx * width, y - ny * width, z - nz * width);
	}

	glEnd();

	ResetMaterial();
}

void DrawTopScarf(
	float length,
	float amplitude,
	float frequency,
	float phase,
	float width,
	float yOffset,
	int segments)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, silk_matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, silk_matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, silk_matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, silk_shininess);

	glBegin(GL_QUAD_STRIP);

	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / segments;
		float x = (t - 0.8f) * length;

		float y = yOffset + amplitude * cosf(frequency * x + phase);
		float z = amplitude * 0.5f * sinf(frequency * x + phase * 0.7f);

		// derivative approximation (tangent)
		float dx = 0.01f;

		float y2 = yOffset + amplitude * cosf(frequency * (x + dx) + phase);
		float z2 = amplitude * 0.5f * sinf(frequency * (x + dx) + phase * 0.7f);

		float tx = dx;
		float ty = y2 - y;
		float tz = z2 - z;

		// normal-ish perpendicular vector in 3D (simple cross trick)
		float nx = -ty;
		float ny = tx;
		float nz = 0.0f;

		float len = sqrtf(nx * nx + ny * ny + nz * nz);
		nx /= len;
		ny /= len;
		nz /= len;

		// two sides of scarf
		glVertex3f(x + nx * width, y + ny * width, z + nz * width);
		glVertex3f(x - nx * width, y - ny * width, z - nz * width);
	}

	glEnd();

	ResetMaterial();
}

void DrawGoldPlate(float width, float height, float depth)
{
	// Upper Cuboid
	glPushMatrix();
	DrawCuboidPolygon(width, height, depth);

	// Lower Prism
	glPushMatrix();
	glTranslatef(0.0f, -(height / 2 + height * 0.2f / 2), 0.0f);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	DrawPrism(width, height * 0.2f, depth);
	glPopMatrix();
	// END Lower Prism

	glPopMatrix();
	// END Upper Cuboid
}

void DrawGoldPlates(float torsoRadius)
{
	float plateWidth = torsoRadius * 1.2f / 3;
	float plateHeight = torsoRadius * 1.1f;
	float plateDepth = 0.005f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, gold_matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gold_matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);

	// Center Plate
	glPushMatrix();
	DrawGoldPlate(plateWidth, plateHeight, plateDepth);
	glPopMatrix();
	// Center Plate

	// Left Plate
	glPushMatrix();
	glTranslatef(-plateWidth, 0.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
	DrawGoldPlate(plateWidth, plateHeight, plateDepth);
	glPopMatrix();
	// Left Plate

	// Right Plate
	glPushMatrix();
	glTranslatef(plateWidth, 0.0f, 0.0f);
	glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
	DrawGoldPlate(plateWidth, plateHeight, plateDepth);
	glPopMatrix();
	// Right Plate

	ResetMaterial();
}

// ***********************
// WEAPONS FUNCTIONS
// ***********************

// ***********************
// CHARACTER FUNCTIONS
// ***********************

// ---------------
// HEAD COMPONENTS
// ---------------

void DrawHeadBase(float baseRadius, float baseHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glScalef(1.1f, 1.0f, 1.2f);
	DrawEnclosedCylinder(quadric, baseRadius, baseRadius, baseHeight, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawUpperHead()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glScalef(1.1f, 1.0f, 1.2f);
	glTranslatef(0.0f, baseHeight * 0.1f, 0.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawLowerHead()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glScalef(1.1f, 0.6f, 1.2f);
	glTranslatef(0.0f, -baseHeight * 0.4f, 0.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void DrawEyeSclera(float side)
{
	float scleraRadius = 0.03f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, scleraTexture);

	glPushMatrix();
	glTranslatef(side * scleraRadius / 2, 0.0f, scleraRadius * 0.2f);
	glRotatef(side * 50.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 0.8f, 0.5f);
	DrawSphere(quadric, scleraRadius, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawEyePupil(float side)
{
	float scleraRadius = 0.03f;
	float pupilRadius = 0.007f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pupilTexture);

	glPushMatrix();
	glTranslatef(side * scleraRadius / 2, 0.0f, (scleraRadius * 0.6f + pupilRadius));
	// glRotatef(side * 35.0f, 0.0f, 1.0f, 0.0f);
	// glScalef(1.0f, 1.0f, 0.2f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawSphere(quadric, pupilRadius, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawEye(float side)
{
	float baseRadius = 0.1f;

	glPushMatrix();
	glTranslatef(side * 0.035f, baseRadius * 0.15f, baseRadius * 0.88f);
	DrawEyeSclera(side);
	DrawEyePupil(side);
	glPopMatrix();
}
void DrawLeftEye() { DrawEye(-1.0f); }
void DrawRightEye() { DrawEye(1.0f); }

void DrawEyelash()
{
	float baseRadius = 0.1f;
	float eyelashLength = 0.02f;
	float eyelashThickness = 0.002f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, eyelashTexture);

	// Middle Eyelash
	glPushMatrix();
	glTranslatef(-0.048f, baseRadius * 0.4f, baseRadius * 1.1f);
	glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, eyelashThickness, eyelashThickness, eyelashLength, SLICES, STACKS);

	// Left Eyelash
	glPushMatrix();
	glTranslatef(0.0f, -eyelashLength, 0.0015f);
	glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, 0.0f, eyelashThickness, eyelashLength, SLICES, STACKS);
	glPopMatrix();
	// END Left Eyelash

	// Right Eyelash
	glPushMatrix();
	glTranslatef(0.0f, eyelashLength, 0.0015f);
	glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, eyelashThickness, 0.0f, eyelashLength, SLICES, STACKS);
	glPopMatrix();
	// END Right Eyelash

	glPopMatrix();
	// END Middle Eyelash

	glDisable(GL_TEXTURE_2D);
}

void DrawLeftEyelash()
{
	glPushMatrix();
	DrawEyelash();
	glPopMatrix();
}
void DrawRightEyelash()
{
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);
	DrawEyelash();
	glPopMatrix();
}

void DrawLip(float length)
{
	float lipLength = length / 3;
	float lipRadius = 0.003f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lipTexture);

	// Middle Lip
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius, lipLength, SLICES, STACKS);

	// Left Lip
	glPushMatrix();
	glTranslatef(-lipRadius * 0.85f, lipLength * 0.95f, 0.0f);
	glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(80.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius * 0.1f, lipLength, SLICES, STACKS);
	glPopMatrix();
	// END Left Lip

	// Right Lip
	glPushMatrix();
	glTranslatef(-lipRadius * 0.85f, -lipLength * 0.95f, 0.0f);
	glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-80.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius * 0.1f, lipLength, SLICES, STACKS);
	glPopMatrix();
	// END Right Lip

	glPopMatrix();
	// END Middle Lip

	glDisable(GL_TEXTURE_2D);
}
void DrawMouth()
{
	float baseRadius = 0.1f;
	float lipLength = baseRadius * 0.75f;

	//// Upper Lip
	// glPushMatrix();
	// glTranslatef(0.0f, -baseRadius * 0.3f, baseRadius * 1.2f);
	// DrawLip(lipLength);
	// glPopMatrix();
	//// END Upper Lip

	// Lower Lip
	glPushMatrix();
	glTranslatef(0.0f, -baseRadius * 0.4f, baseRadius * 1.2f);
	glScalef(1.0f, -1.0f, 1.0f);
	DrawLip(lipLength);
	glPopMatrix();
	// END Lower Lip
}

void DrawEar(float side)
{
	float baseRadius = 0.1f;
	float earWidth = 0.05f;
	float earHeight = 0.1f;
	float earDepth = 0.05f;
	float earTopOffset = 0.01f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();

	glRotatef(-side * 70.0f, 0.0f, 0.0f, 1.0f);
	DrawPrismWithOffset(earWidth, earHeight, earDepth, earTopOffset);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawLeftEar() { DrawEar(-1.0f); }
void DrawRightEar() { DrawEar(1.0f); }

void DrawNoseSide(float side)
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	float noseBaseRadius = baseRadius * 0.05f;
	float noseBaseHeight = baseHeight * 0.15f;
	float noseThickness = 0.7f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();

	glTranslatef(noseBaseRadius * 0.5f * side, 0.0f, baseRadius * 1.2f);
	glRotatef(15.0f * side, 0.0f, 0.0f, 1.0f);
	glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

	DrawEnclosedSemiCylinderWithThickness(
		quadric,
		noseBaseRadius,
		noseBaseRadius * 0.5f,
		noseBaseHeight,
		noseThickness,
		SLICES, STACKS, LOOPS);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawNoseMiddle()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	float noseBaseRadius = baseRadius * 0.05f;
	float noseBaseHeight = baseHeight * 0.1f;
	float noseThickness = 0.7f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();

	glTranslatef(0.0f, noseBaseHeight * 0.3f, baseRadius * 1.2f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

	DrawEnclosedSemiCylinderWithThickness(
		quadric,
		noseBaseRadius * 2.0f,
		noseBaseRadius * 0.22f,
		noseBaseHeight * 2.0f,
		noseThickness,
		SLICES, STACKS, LOOPS);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void DrawNose()
{
	DrawNoseSide(-1.0f);
	DrawNoseSide(1.0f);
	DrawNoseMiddle();
}

// ----------------
// TORSO COMPONENTS
// ----------------

void DrawTorsoPart(float baseRadius, float baseHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Center Base
	glPushMatrix();
	glScalef(1.5f, 1.0f, 0.5f);
	DrawEnclosedCylinder(quadric, baseRadius, baseRadius, baseHeight, SLICES, STACKS);

	// Sholder / Pelvis
	glPushMatrix();
	glTranslatef(0.0f, baseHeight * 0.5f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();
	// END Sholder / Pelvis

	glPopMatrix();
	// END Center Base

	glDisable(GL_TEXTURE_2D);
}

// --------------
// ARM COMPONENTS
// --------------

void DrawUpperArm(float armLength)
{
	float baseRadius = 0.03f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Upper Arm Joint
	glPushMatrix();
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Upper Arm
	glPushMatrix();
	glTranslatef(-armLength / 2, 0.0f, 0.0f);
	glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.8f, baseRadius * 0.45f, armLength, SLICES, STACKS);
	glPopMatrix();
	// END Upper Arm

	glPopMatrix();
	// END Upper Arm Joint

	glDisable(GL_TEXTURE_2D);
}

void DrawLowerArm(float armLength)
{
	float upperBaseRadius = 0.03f;
	float baseRadius = 0.02f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Lower Arm Joint
	glPushMatrix();
	DrawSphere(quadric, upperBaseRadius * 0.6f, SLICES, STACKS);

	// Lower Arm
	glPushMatrix();
	glTranslatef(-armLength / 2, 0.0f, 0.0f);
	glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.7f, baseRadius * 0.3f, armLength, SLICES, STACKS);
	glPopMatrix();
	// END Lower Arm

	glPopMatrix();
	// END Lower Arm Joint

	glDisable(GL_TEXTURE_2D);
}

void DrawFinger(float length, float bendAngle) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Finger Joint 1
	float joint1Radius = 0.005f;
	glPushMatrix();
	DrawSphere(quadric, joint1Radius, SLICES, STACKS);

	// Finger 1
	float finger1Length = length * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, finger1Length / 2);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, joint1Radius * 0.8f, joint1Radius * 0.5f, finger1Length, SLICES, STACKS);

	// Finger Joint 2
	float joint2Radius = joint1Radius * 0.8f;
	glPushMatrix();
	glTranslatef(0.0f, finger1Length / 2, 0.0f);
	DrawSphere(quadric, joint2Radius, SLICES, STACKS);

	// Finger 2
	float finger2Length = length / 2;
	glPushMatrix();
	glTranslatef(0.0f, finger1Length / 2 + finger2Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint2Radius * 0.8f, joint2Radius * 0.5f, finger2Length, SLICES, STACKS);

	// Finger Joint 3
	float joint3Radius = joint2Radius * 0.8f;
	glPushMatrix();
	glTranslatef(0.0f, finger2Length / 2, 0.0f);
	DrawSphere(quadric, joint3Radius, SLICES, STACKS);

	// Finger 3
	float finger3Length = length / 2;
	glPushMatrix();
	glTranslatef(0.0f, finger3Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint3Radius * 0.8f, joint3Radius * 0.5f, finger3Length, SLICES, STACKS);

	// Finger Tip [Claw]
	float clawRadius = joint3Radius * 0.7f;
	float clawHeight = joint3Radius * 2.0f;
	glPushMatrix();
	glTranslatef(0.0f, finger3Length / 2, 0.0f);
	DrawCylinder(quadric, clawRadius, 0.0f, clawHeight, SLICES, STACKS);
	glPopMatrix();
	// END Finger Tip [Claw]

	glPopMatrix();
	// END Finger 3

	glPopMatrix();
	// END Finger Joint 3

	glPopMatrix();
	// END Finger 2

	glPopMatrix();
	// END Finger Joint 2

	glPopMatrix();
	// END Finger 1

	glPopMatrix();
	// END Finger Joint 1

	glDisable(GL_TEXTURE_2D);
}

void DrawHand() {
	float baseRadius = 0.018f;
	float palmSize = 0.018f;

	float thumbLength = 0.015f;
	float indexLength = 0.017f;
	float middleLength = 0.019f;
	float ringLength = indexLength;
	float littleLength = 0.014f;

	float fingerSpacing = 0.0065f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Palm
	glPushMatrix();
	glScalef(0.9f, 0.5f, 1.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Reset scale
	glScalef(1.1f, 1.5f, 1.0f);

	// Thumb Finger
	glPushMatrix();
	glTranslatef(baseRadius * 0.3f, 0.0f, baseRadius * 0.8f);
	glRotatef(-70.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip! (Multiplied by 0.5 so thumb bends a bit less)
	DrawFinger(thumbLength, currentGrip * 0.5f);
	glPopMatrix();
	// END Thumb Finger

	// Index Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + indexLength / 3), 0.0f, fingerSpacing);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(indexLength, currentGrip);
	glPopMatrix();
	// END Index Finger

	// Middle Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + middleLength / 2), 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(middleLength, currentGrip);
	glPopMatrix();
	// END Middle Finger

	// Ring Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + ringLength / 3), 0.0f, -fingerSpacing);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(ringLength, currentGrip);
	glPopMatrix();
	// END Ring Finger

	// Little Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2), 0.0f, -fingerSpacing * 2);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(littleLength, currentGrip);
	glPopMatrix();
	// END Little Finger

	glPopMatrix();
	// END Palm

	glDisable(GL_TEXTURE_2D);
}

void DrawArm(float side) {
	float upperArmLength = 0.06f;
	float lowerArmLength = 0.08f;

	glRotatef((side + 1.0f) * 90.0f, 0.0f, 0.0f, 1.0f);

	// Upper Arm
	glPushMatrix();
	PartRotation &upperArm =
		(side < 0) ? parts[LEFT_UPPER_ARM] : parts[RIGHT_UPPER_ARM];
	glRotatef(upperArm.angleX, 1, 0, 0);
	glRotatef(upperArm.angleY, 0, 1, 0);
	glRotatef(upperArm.angleZ, 0, 0, 1);
	DrawUpperArm(upperArmLength);

	// Lower Arm
	glPushMatrix();
	glTranslatef(-(upperArmLength / 2 + lowerArmLength / 2), 0.0f, 0.0f);
	PartRotation &lowerArm =
		(side < 0) ? parts[LEFT_LOWER_ARM] : parts[RIGHT_LOWER_ARM];
	glRotatef(lowerArm.angleX, 1, 0, 0);
	glRotatef(lowerArm.angleY, 0, 1, 0);
	glRotatef(lowerArm.angleZ, 0, 0, 1);
	DrawLowerArm(lowerArmLength);

	// Hand
	glPushMatrix();
	glTranslatef(-lowerArmLength, 0.0f, 0.0f);
	PartRotation &hand =
		(side < 0) ? parts[LEFT_HAND] : parts[RIGHT_HAND];
	glRotatef(hand.angleX, 1, 0, 0);
	glRotatef(hand.angleY, 0, 1, 0);
	glRotatef(hand.angleZ, 0, 0, 1);
	DrawHand(); // <-- Your hand is drawn here

	// ==========================================
	// WEAPON CYCLING LOGIC (Only on Right Arm)
	// ==========================================
	if (side > 0.0f) {
		glPushMatrix();

		switch (currentWeapon) {
		case 1: // --- SPEAR ---
			// 1. Position: X is slightly negative to align with the palm center, 
			// Y is 0.0f to center it, Z pushes it slightly forward into the fingers.


			glTranslatef(0.00, -0.05f, -0.18f);

			// 2. Scale
			glScalef(0.2f, 0.2f, 0.2f);

			// 3. Rotation: Make it vertical
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

			// 4. CENTER IT: Slide the spear backward by half its length
			glTranslatef(0.0f, 0.0f, -0.35f);

			DrawSpear(0.7);
			break;

		case 2: // --- WEAPON 2 (Example) ---
			// 1. Position: X is slightly negative to align with the palm center, 
			// Y is 0.0f to center it, Z pushes it slightly forward into the fingers.
			glTranslatef(-0.07f, 0.01f, 0.25f);
			glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			// 2. Scale
			glScalef(0.2f, 0.2f, 0.2f);
			// 3. Rotation: Make it vertical
			// 4. CENTER IT: Slide the spear backward by half its length
			glTranslatef(0.0f, 0.0f, -0.35f);

			DrawFishSword(0.7);

			break;

		case 0: // --- NO WEAPON ---
		default:
			break;
		}

		glPopMatrix();
	}
	// ==========================================

	glPopMatrix(); // END Hand
	glPopMatrix(); // END Lower Arm
	glPopMatrix(); // END Upper Arm
}

// --------------
// LEG COMPONENTS
// --------------

void DrawUpperLeg(float legLength)
{
	float baseRadius = 0.03f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Upper Leg Joint
	glPushMatrix();
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Upper Leg
	glPushMatrix();
	glTranslatef(0.0f, -legLength / 2, 0.0f);
	// glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.45f, baseRadius * 0.8f, legLength, SLICES, STACKS);
	glPopMatrix();
	// END Upper Leg

	glPopMatrix();
	// END Upper Leg Joint

	glDisable(GL_TEXTURE_2D);
}

void DrawLowerLeg(float legLength)
{
	float upperBaseRadius = 0.03f;
	float baseRadius = 0.02f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Lower Leg Joint
	glPushMatrix();
	DrawSphere(quadric, upperBaseRadius * 0.6f, SLICES, STACKS);

	// Lower Leg
	glPushMatrix();
	glTranslatef(0.0f, -legLength / 2, 0.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.3f, baseRadius * 0.7f, legLength, SLICES, STACKS);
	glPopMatrix();
	// END Lower Leg

	glPopMatrix();
	// END Lower Leg Joint

	glDisable(GL_TEXTURE_2D);
}

void DrawToe(float length, float joint1Radius, boolean isBigToe)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glScalef(1.0f, 1.0f, 0.8f);

	// Toe Joint 1
	glPushMatrix();
	DrawSphere(quadric, joint1Radius, SLICES, STACKS);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	// Toe 1
	float toe1Length = length * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, toe1Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint1Radius * 0.8f, joint1Radius * 0.5f, toe1Length, SLICES, STACKS);

	// Toe Joint 2
	float joint2Radius = joint1Radius * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, toe1Length / 2, 0.0f);
	DrawSphere(quadric, joint2Radius, SLICES, STACKS);

	// Toe 2
	float toe2Length = length * 0.3f;
	glPushMatrix();
	glTranslatef(0.0f, toe2Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint2Radius * 0.8f, joint2Radius * 0.5f, toe2Length, SLICES, STACKS);

	if (!isBigToe)
	{
		// Toe Joint 3
		float joint3Radius = joint2Radius * 0.8f;
		glPushMatrix();
		glTranslatef(0.0f, toe2Length / 2, 0.0f);
		DrawSphere(quadric, joint3Radius, SLICES, STACKS);

		// Toe 3
		float toe3Length = length / 2;
		glPushMatrix();
		glTranslatef(0.0f, joint3Radius + toe3Length / 2, 0.0f);
		DrawEnclosedCylinder(quadric, joint3Radius * 0.8f, joint3Radius * 0.5f, toe3Length, SLICES, STACKS);

		// Toe Tip
		glPushMatrix();
		glTranslatef(0.0f, toe3Length / 2, 0.0f);
		DrawSphere(quadric, joint3Radius * 0.5f, SLICES, STACKS);
		glPopMatrix();
		// END Toe Tip

		glPopMatrix();
		// END Toe 3

		glPopMatrix();
		// END Toe Joint 3
	}
	else
	{
		// Toe Tip
		glPushMatrix();
		glTranslatef(0.0f, toe2Length / 2, 0.0f);
		DrawSphere(quadric, joint2Radius * 0.5f, SLICES, STACKS);
		glPopMatrix();
		// END Toe Tip
	}

	glPopMatrix();
	// END Toe 2

	glPopMatrix();
	// END Toe Joint 2

	glPopMatrix();
	// END Toe 1

	glPopMatrix();
	// END Toe Joint 1

	glDisable(GL_TEXTURE_2D);
}

void DrawFoot()
{
	float ankleRadius = 0.01f;

	float bigToeLength = 0.01f;
	float indexToeLength = 0.015f;
	float middleToeLength = 0.01f;
	float ringToeLength = 0.008f;
	float littleToeLength = 0.007f;

	float bigToeRadius = 0.004f;
	float indexToeRadius = 0.003f;
	float middleToeRadius = 0.003f;
	float ringToeRadius = 0.003f;
	float littleToeRadius = 0.003f;

	float toeSpacing = 0.01f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Ankle
	glPushMatrix();
	glScalef(1.0f, 0.7f, 1.2f);
	DrawSphere(quadric, ankleRadius, SLICES, STACKS);

	glScalef(1.0f, 1.3f, 0.5f);

	// Midfoot
	float midfootWidth = 0.1f;
	float midfootHeight = 0.015f;
	float midfootDepth = 0.03f;
	float midfootTopOffset = midfootWidth * 0.1f;
	glPushMatrix();
	glTranslatef(0.0f, -ankleRadius, ankleRadius + midfootTopOffset * 5);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawRightTrapezoidalPrism(midfootWidth, midfootHeight, midfootDepth, midfootTopOffset);

	// Midfoot [BACK]
	float midfootBottomRadius = midfootDepth / 2;
	float midfootTopRadius = midfootTopOffset;
	glPushMatrix();
	glTranslatef(-midfootWidth / 2, 0.0f, 0.0f);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(5.5f, 1.0f, 1.5f);
	DrawEnclosedCylinder(quadric, midfootBottomRadius, midfootTopRadius, midfootHeight, SLICES, STACKS);
	glPopMatrix();
	// END Midfoot [BACK]

	float offsetX = (midfootWidth - midfootTopOffset) / 2;
	glTranslatef(offsetX, -midfootHeight / 4, -toeSpacing * 0.2f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 2.0f);

	// Big Toe
	glPushMatrix();
	glTranslatef(-toeSpacing * 1.2f, 0.0f, 0.0f);
	DrawToe(bigToeLength, bigToeRadius, true);
	glPopMatrix();
	// END Big Toe

	// Index Toe
	glPushMatrix();
	glTranslatef(-toeSpacing * 0.5f, 0.0f, 0.0f);
	DrawToe(indexToeLength, indexToeRadius, true);
	glPopMatrix();
	// END Index Toe

	// Middle Toe
	glPushMatrix();
	DrawToe(middleToeLength, middleToeRadius, true);
	glPopMatrix();
	// END Middle Toe

	// Ring Toe
	glPushMatrix();
	glTranslatef(toeSpacing * 0.5f, 0.0f, 0.0f);
	DrawToe(ringToeLength, ringToeRadius, true);
	glPopMatrix();
	// END Ring Toe

	// Little Toe
	glPushMatrix();
	glTranslatef(toeSpacing, 0.0f, 0.0f);
	DrawToe(littleToeLength, littleToeRadius, true);
	glPopMatrix();
	// END Little Toe

	glPopMatrix();
	// END Midfoot

	glPopMatrix();
	// END Ankle

	extern int currentWeapon; // (Optional: depending on where currentWeapon is declared, you might need this)

	if (currentWeapon == 3) {
		glDisable(GL_TEXTURE_2D); // Turn off skin texture for the fiery colors

		glPushMatrix();
		// 1. Position: Y moves it down under the sole, Z pushes it forward to center under the midfoot
		glTranslatef(0.0f, -0.1f, 0.04f);

		// 2. Rotate it so it stands upright and rolls forward
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

		// 3. Draw the wheel
		DrawWindFireWheel(0.04f);
		glPopMatrix();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, skinTexture);
	}
	// ==========================================

	glDisable(GL_TEXTURE_2D);
}

void DrawLeg(float side, float torsoRadius)
{
	float upperLegLength = 0.06f;
	float lowerLegLength = 0.07f;

	glScalef(side, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Upper Leg
	glPushMatrix();

	PartRotation &upperLeg =
		(side < 0) ? parts[LEFT_UPPER_LEG] : parts[RIGHT_UPPER_LEG];
	glRotatef(upperLeg.angleX, 1, 0, 0);
	glRotatef(upperLeg.angleY, 0, 1, 0);
	glRotatef(upperLeg.angleZ, 0, 0, 1);

	glRotatef(0.0f, 0.0f, (side + 1.0f) * 90.0f, 1.0f);
	DrawUpperLeg(upperLegLength);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	// Upper Pant Leg
	float legRadius = 0.03f;

	glPushMatrix();
	glTranslatef(0.0f, -upperLegLength / 2, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, legRadius * 0.8f, legRadius, upperLegLength, SLICES, STACKS);
	glPopMatrix();
	// END Upper Pant Leg

	// Pant Joint
	float pantJointRadius = legRadius * 1.1f;
	glPushMatrix();
	glTranslatef(0.0f, -upperLegLength, 0.0f);
	glScalef(1.0f, 0.5f, 1.0f);
	DrawSphere(quadric, pantJointRadius, SLICES, STACKS);

	// Gold Plate

	// END Gold Plate

	glPopMatrix();
	// END Pant Joint

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Lower Leg
	glPushMatrix();
	glTranslatef(0.0f, -(upperLegLength / 2 + lowerLegLength / 2), 0.0f);
	PartRotation &lowerLeg =
		(side < 0) ? parts[LEFT_LOWER_LEG] : parts[RIGHT_LOWER_LEG];
	glRotatef(lowerLeg.angleX, 1, 0, 0);
	glRotatef(lowerLeg.angleY, 0, 1, 0);
	glRotatef(lowerLeg.angleZ, 0, 0, 1);
	DrawLowerLeg(lowerLegLength);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	// Lower Pant Leg
	glPushMatrix();
	glTranslatef(0.0f, -lowerLegLength * 0.4f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, legRadius * 0.35f, legRadius * 0.75f, lowerLegLength * 0.7f, SLICES, STACKS);

	// Hem
	float hemRadius = lowerLegLength * 0.13f;
	glPushMatrix();
	glTranslatef(0.0f, -(lowerLegLength * 0.7f) / 2, 0.0f);
	DrawSphere(quadric, hemRadius, SLICES, STACKS);

	// Opening
	float openingRadius = hemRadius * 1.0f;
	float openingHeight = lowerLegLength * 0.05f;
	glPushMatrix();
	glTranslatef(0.0f, -hemRadius * 0.7f, 0.0f);
	DrawEnclosedCylinder(quadric, openingRadius, openingRadius, openingHeight, SLICES, STACKS);
	glPopMatrix();
	// END Opening

	glPopMatrix();
	// END Hem

	glPopMatrix();
	// END Lower Pant Leg

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Foot
	glPushMatrix();
	glTranslatef(0.0f, -lowerLegLength, 0.0f);
	PartRotation &foot =
		(side < 0) ? parts[LEFT_FOOT] : parts[RIGHT_FOOT];
	glRotatef(foot.angleX, 1, 0, 0);
	glRotatef(foot.angleY, 0, 1, 0);
	glRotatef(foot.angleZ, 0, 0, 1);
	DrawFoot();
	glPopMatrix();
	// END Foot

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	// END Lower Leg

	glPopMatrix();
	// END Upper Leg
}

// ========================
// CHARACTER PARTS ASSEMBLY
// ========================

void DrawHead(float headBaseRadius, float headBaseHeight)
{
	// Head
	glPushMatrix();
	DrawHeadBase(headBaseRadius, headBaseHeight);
	DrawUpperHead();
	DrawLowerHead();

	// Eyes
	glPushMatrix();
	DrawLeftEye();
	DrawRightEye();
	glPopMatrix();
	// END Eyes

	// Eyelashes
	glPushMatrix();
	DrawLeftEyelash();
	DrawRightEyelash();
	glPopMatrix();
	// END Eyelashes

	// Mouth
	glPushMatrix();
	DrawMouth();
	glPopMatrix();
	// END Mouth

	// Left Ear
	glPushMatrix();
	glTranslatef(-headBaseRadius * 1.2f, 0.0f, 0.0f);
	DrawLeftEar();

	// Left Ear Ring
	glPushMatrix();
	DrawEarRing(-1.0f);
	glPopMatrix();
	// END Left Ear Ring

	glPopMatrix();
	// END Left Ear

	// Right Ear
	glPushMatrix();
	glTranslatef(headBaseRadius * 1.2f, 0.0f, 0.0f);
	DrawRightEar();
	DrawEarRing(1.0f);
	glPopMatrix();
	// END Right Ear

	// Nose
	glPushMatrix();
	DrawNose();
	glPopMatrix();
	// END Nose

	glPopMatrix();
	// END Head
}

void DrawNeck(float neckRadius, float neckHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);
	glPushMatrix();
	DrawEnclosedCylinder(quadric, neckRadius, neckRadius, neckHeight, SLICES, STACKS);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void DrawTorso(float torsoRadius, float torsoHeight)
{
	// Upper Torso
	glPushMatrix();
	DrawTorsoPart(torsoRadius, torsoHeight);
	glPopMatrix();
	// END Upper Torso

	// Lower Torso
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight, 0.0f);
	glScalef(1.0f, -1.0f, 1.0f);
	DrawTorsoPart(torsoRadius, torsoHeight);
	glPopMatrix();
	// END Lower Torso
}

void DrawArms()
{
	float torsoArmsOffsetX = 0.095f;

	// Left Arm
	glPushMatrix();
	glTranslatef(-torsoArmsOffsetX, 0.0f, 0.0f);
	glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(80.0f, 0.0f, 0.0f, 1.0f);
	DrawArm(-1.0f);
	glPopMatrix();
	// END Left Arm

	// Right Arm
	glPushMatrix();
	glTranslatef(torsoArmsOffsetX, 0.0f, 0.0f);
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-80.0f, 0.0f, 0.0f, 1.0f);
	DrawArm(1.0f);
	glPopMatrix();
	// END Right Arm
}

void DrawLegs(float torsoRadius)
{
	float torsoLegOffsetX = 0.04f;

	// Left Leg
	glPushMatrix();
	glTranslatef(-torsoLegOffsetX, 0.0f, 0.0f);
	// glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	DrawLeg(-1.0f, torsoRadius);
	glPopMatrix();
	// END Left Leg

	// Right Leg
	glPushMatrix();
	glTranslatef(torsoLegOffsetX, 0.0f, 0.0f);
	// glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
	// glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	DrawLeg(1.0f, torsoRadius);

	glPopMatrix();
	// END Right Leg
}

// ***********************
// HAIR FUNCTIONS
// ***********************

void DrawHairStripsSide(float hairRadius, int numStrips, float side)
{
	float stripTubeRadius = (hairRadius * 1.4f / numStrips) / 2;
	float stripEdgeRadius = stripTubeRadius * 0.5f;
	float stripBentAngle = 195.0f;
	float stripArcRadius = hairRadius * 0.985f;
	int stripSegments = 15;

	float arcRadiusFactor = 0.98f;
	float bentAngleFactor = 0.995f;

	glPushMatrix();
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 0.95f, 1.0f);

	for (int i = 0; i < numStrips; i++)
	{
		DrawEnclosedBentCylinder(quadric, stripTubeRadius, stripEdgeRadius, stripArcRadius, stripBentAngle, stripSegments, SLICES, STACKS);

		stripArcRadius *= arcRadiusFactor;
		stripBentAngle *= bentAngleFactor;

		glTranslatef(0.0f, 0.0f, side * -stripTubeRadius);
	}

	glPopMatrix();
}

void DrawHairStripsSide(float hairRadius, float side)
{
	float stripWidth = hairRadius * 0.4f;
	float stripHeight = 0.1f;
	float stripDepth = hairRadius * 0.2f;
	float stripTopOffset = stripWidth * 0.5f;

	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

	float stripOffsetX = stripWidth / 4;

	for (int i = 0; i < 2; i++)
	{
		// Strip 1
		glPushMatrix();
		glTranslatef(side * stripOffsetX, hairRadius * 0.9f, hairRadius * 0.2f);
		glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
		DrawPyramid(stripWidth, stripHeight);
		glPopMatrix();
		// END Strip 1

		// Strip 2
		glPushMatrix();
		glTranslatef(side * stripOffsetX, hairRadius, 0.0f);
		glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
		DrawPyramid(stripWidth, stripHeight);
		glPopMatrix();
		// END Strip 2

		// Strip 3
		glPushMatrix();
		glTranslatef(side * stripOffsetX, hairRadius * 0.9f, -hairRadius * 0.3f);
		glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
		DrawPyramid(stripWidth, stripHeight);
		glPopMatrix();
		// END Strip 3

		stripOffsetX *= 4;
	}
}

void DrawHairStrips(float hairRadius)
{
	// Left Hair Strips
	glPushMatrix();
	DrawHairStripsSide(hairRadius, -1.0f);
	glPopMatrix();
	// END Left Hair Strips

	// Right Hair Strips
	glPushMatrix();
	DrawHairStripsSide(hairRadius, 1.0f);
	glPopMatrix();
	// END Right Hair Strips
}

void DrawHair(float headBaseRadius)
{
	float hairRadius = headBaseRadius * 1.2f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, hairTexture);

	// Hair Scalp
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 1.0f, 1.025f);
	DrawSemiSphere(quadric, hairRadius, SLICES, STACKS);

	// Hair Strips
	glPushMatrix();
	DrawHairStrips(hairRadius);
	glPopMatrix();
	// END Hair Strips

	float sideHairTopRadius = hairRadius * 0.6f;

	// Side Hair [LEFT]
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.47f, 0.0f);
	glScalef(1.3f, 1.0f, 1.1f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, sideHairTopRadius, SLICES, STACKS);
	glPopMatrix();
	// END Side Hair [LEFT]

	// Side Hair [RIGHT]
	glPushMatrix();
	glTranslatef(0.0f, -headBaseRadius * 0.47f, 0.0f);
	glScalef(1.3f, 1.0f, 1.1f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, sideHairTopRadius, SLICES, STACKS);
	glPopMatrix();
	// END Side Hair [RIGHT]

	// Back Hair [TOP]
	float topBackHairRadius = hairRadius * 0.9f;
	glPushMatrix();
	glTranslatef(-topBackHairRadius * 0.5f, 0.0f, -hairRadius * 0.2f);
	glRotatef(80.0f, 0.0f, 1.0f, 0.0f);
	DrawSemiSphere(quadric, topBackHairRadius, SLICES, STACKS);
	glPopMatrix();
	// END Back Hair [TOP]

	// Back Hair [BOTTOM]
	float bottomBackHairRadius = hairRadius * 0.9f;
	glPushMatrix();
	glTranslatef(-bottomBackHairRadius * 1.2f, 0.0f, -hairRadius * 0.22f);
	glRotatef(93.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.5f);
	DrawSemiSphere(quadric, bottomBackHairRadius, SLICES, STACKS);
	glPopMatrix();
	// END Back Hair [BOTTOM]

	float bangRadius = hairRadius * 0.28f;
	float bangHeight = bangRadius * 0.2f;
	float bangThickness = 0.7f;

	glScalef(1.3f, 1.0f, 1.2f);

	// Left Bun
	glPushMatrix();
	glTranslatef(hairRadius * 0.6f, hairRadius, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.2f, 1.2f, 1.0f);
	DrawEnclosedCylinderWithThickness(quadric, bangRadius, bangRadius, bangHeight, bangThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Left Bun

	// Right Bun
	glPushMatrix();
	glTranslatef(hairRadius * 0.6f, -hairRadius, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.2f, 1.2f, 1.0f);
	DrawEnclosedCylinderWithThickness(quadric, bangRadius, bangRadius, bangHeight, bangThickness, SLICES, STACKS, LOOPS);
	glPopMatrix();
	// END Right Bun

	glPopMatrix();
	// END Hair Scalp

	glDisable(GL_TEXTURE_2D);
}

// ***********************
// OUTFITS FUNCTIONS
// ***********************

void DrawRedVest(float torsoRadius, float torsoHeight)
{
	float vestRadius = torsoRadius * 1.2f;
	float vestHeight = torsoRadius * 1.8f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, redBlackFlameTexture);

	// Left Side Vest
	glPushMatrix();
	glTranslatef(-torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(-3.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Left Side Top Semi Cylinder
	float topVestHeight = vestHeight * 0.2f;
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, vestRadius * 1.5f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Left Side Top Semi Cylinder

	glPopMatrix();
	// END Left Side Vest

	// Right Side Vest
	glPushMatrix();
	glTranslatef(torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Right Side Top Semi Cylinder
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, vestRadius * 1.5f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Right Side Top Semi Cylinder

	glPopMatrix();
	// END Right Side Vest

	// Back Side Vest
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -torsoRadius / 2);
	glScalef(1.0f, 1.62f, 0.9f);
	DrawCuboidPolygon(torsoRadius, vestHeight, torsoRadius);
	glPopMatrix();
	// END Back Side Vest

	glDisable(GL_TEXTURE_2D);
}

void DrawPantLeg(float side, float torsoRadius)
{
	float legRadius = 0.03f;
	float legHeight = 0.1f;

	// Pant Leg
	glPushMatrix();
	glTranslatef(-(torsoRadius / 2 + legHeight * 0.3f), -side * (torsoRadius * 0.5f + legRadius * 0.35f), 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, legRadius * 0.4f, legRadius, legHeight, SLICES, STACKS);

	// Hem
	float hemRadius = legHeight * 0.118f;
	glPushMatrix();
	glTranslatef(0.0f, -legHeight / 2, 0.0f);
	DrawSphere(quadric, hemRadius, SLICES, STACKS);

	// Opening
	float openingRadius = hemRadius * 1.0f;
	float openingHeight = legHeight * 0.05f;
	glPushMatrix();
	glTranslatef(0.0f, -hemRadius * 0.8f, 0.0f);
	DrawEnclosedCylinder(quadric, openingRadius, openingRadius, openingHeight, SLICES, STACKS);
	glPopMatrix();
	// END Opening

	glPopMatrix();
	// END Hem

	glPopMatrix();
	// END Pant Leg
}

void DrawPants(float torsoRadius)
{
	float upperPantRadius = 0.034f;
	float upperPantHeight = torsoRadius * 2.5f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	// Waistband
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, upperPantRadius, upperPantRadius, upperPantHeight, SLICES, STACKS);

	// Pant Leg [LEFT]
	glPushMatrix();
	DrawPantLeg(-1.0f, torsoRadius);
	glPopMatrix();
	// END Pant Leg [LEFT]

	// Pant Leg [RIGHT]
	glPushMatrix();
	DrawPantLeg(1.0f, torsoRadius);
	glPopMatrix();
	// END Pant Leg [RIGHT]

	glPopMatrix();
	// END Waistband

	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementHeadJewel(float headBaseRadius)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({0.88f, 0.76f, 0.38f});

	glPushMatrix();
	glTranslatef(headBaseRadius * 0.28f, headBaseRadius * 0.48f, headBaseRadius * 0.74f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.11f, headBaseRadius * 0.11f, headBaseRadius * 0.035f, 0.18f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(headBaseRadius * 0.18f, headBaseRadius * -0.62f, headBaseRadius * 0.72f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.06f, headBaseRadius * 0.06f, headBaseRadius * 0.03f, 0.2f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({0.42f, 0.78f, 1.0f});

	glPushMatrix();
	glTranslatef(headBaseRadius * 0.13f, headBaseRadius * -0.62f, headBaseRadius * 0.72f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.6f, 0.6f, 0.4f);
	DrawSphere(quadric, headBaseRadius * 0.08f, SLICES, STACKS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(headBaseRadius * 0.02f, headBaseRadius * -0.62f, headBaseRadius * 0.72f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.18f, 1.0f, 0.18f);
	DrawCuboidPolygon(headBaseRadius * 0.1f, headBaseRadius * 0.34f, headBaseRadius * 0.1f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementFrontRobe(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({0.22f, 0.34f, 0.46f});

	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 0.05f, -torsoRadius * 0.88f);
	glRotatef(2.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.9f, 1.45f, 0.24f);
	DrawCuboidPolygon(torsoRadius * 1.0f, torsoHeight * 2.0f, torsoRadius * 0.18f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-torsoRadius * 0.35f, torsoHeight * 0.05f, torsoRadius * 1.04f);
	glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.45f, 1.25f, 0.16f);
	DrawCuboidPolygon(torsoRadius * 0.16f, torsoHeight * 1.25f, torsoRadius * 0.08f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(torsoRadius * 0.4f, -torsoHeight * 0.95f, -torsoRadius * 0.75f);
	glRotatef(-16.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.42f, 1.6f, 0.14f);
	DrawCuboidPolygon(torsoRadius * 0.3f, torsoHeight * 1.6f, torsoRadius * 0.1f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementGoldTrim(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({0.88f, 0.76f, 0.36f});

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.68f, torsoRadius * 1.04f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.34f, torsoRadius * 0.34f, torsoRadius * 0.06f, 0.16f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 0.95f, torsoHeight * 0.8f, torsoRadius * 0.15f);
		glRotatef((float)(-i * 20), 0.0f, 0.0f, 1.0f);
		glScalef(1.0f, 0.75f, 0.7f);
		DrawSemiSphere(quadric, torsoRadius * 0.28f, SLICES, STACKS);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.22f, torsoRadius * 1.08f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.16f, torsoRadius * 0.16f, torsoRadius * 0.05f, 0.25f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementSleeves(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, silverTexture);
	ApplyTint({0.95f, 0.96f, 1.0f});

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 1.02f, torsoHeight * 0.15f, torsoRadius * 0.35f);
		glRotatef((float)(i * 18), 0.0f, 0.0f, 1.0f);
		glScalef(0.3f, 1.0f, 0.28f);
		DrawCuboidPolygon(torsoRadius * 0.22f, torsoHeight * 1.0f, torsoRadius * 0.1f);
		glPopMatrix();
	}

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementGem(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({0.48f, 0.82f, 1.0f});

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.2f, torsoRadius * 1.1f);
	glScalef(0.4f, 0.75f, 0.18f);
	DrawCuboidPolygon(torsoRadius * 0.12f, torsoHeight * 0.35f, torsoRadius * 0.08f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementDecorations(float torsoRadius, float torsoHeight)
{
	DrawWaterElementFrontRobe(torsoRadius, torsoHeight);
	DrawWaterElementGoldTrim(torsoRadius, torsoHeight);
	DrawWaterElementGem(torsoRadius, torsoHeight);
	DrawWaterElementSleeves(torsoRadius, torsoHeight);
}

void DrawWoodElementHeadAccessory(float headBaseRadius)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * headBaseRadius * 1.12f, headBaseRadius * 0.42f, -headBaseRadius * 0.06f);
		glRotatef((float)(-i * 22), 0.0f, 0.0f, 1.0f);
		glRotatef(24.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.18f, 0.95f, 0.18f);
		DrawCuboidPolygon(headBaseRadius * 0.24f, headBaseRadius * 0.62f, headBaseRadius * 0.24f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, grassTexture);
	ApplyTint({ 0.40f, 0.78f, 0.34f });

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * headBaseRadius * 0.92f, headBaseRadius * 0.20f, headBaseRadius * 0.82f);
		glRotatef((float)(-i * 26), 0.0f, 0.0f, 1.0f);
		glRotatef(18.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.58f, 1.15f, 0.18f);
		DrawCuboidPolygon(headBaseRadius * 0.10f, headBaseRadius * 0.30f, headBaseRadius * 0.08f);
		glPopMatrix();
	}

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWoodElementDecorations(float torsoRadius, float torsoHeight, bool isChibi)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 1.42f, torsoHeight * 0.72f, torsoRadius * 0.10f);
		glRotatef((float)(i * 16), 0.0f, 0.0f, 1.0f);
		glRotatef(8.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.30f, 0.82f, 0.46f);
		DrawCuboidPolygon(torsoRadius * 0.16f, torsoHeight * 0.58f, torsoRadius * 0.10f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, grassTexture);
	ApplyTint({ 0.40f, 0.78f, 0.34f });

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 1.18f, torsoHeight * 0.26f, torsoRadius * 0.52f);
		glRotatef((float)(-i * 26), 0.0f, 0.0f, 1.0f);
		glScalef(0.42f, 0.92f, 0.16f);
		DrawCuboidPolygon(torsoRadius * 0.14f, torsoHeight * 0.50f, torsoRadius * 0.06f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.72f, 0.58f, 0.22f });
	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.18f, torsoRadius * 1.12f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.18f, torsoRadius * 0.18f, torsoRadius * 0.05f, 0.26f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWoodElementHairDecorations(float headBaseRadius)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	ApplyTint({ 0.44f, 0.80f, 0.36f });

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * headBaseRadius * 1.04f, headBaseRadius * 0.12f, headBaseRadius * 0.20f);
		glRotatef((float)(-i * 20), 0.0f, 0.0f, 1.0f);
		glScalef(0.24f, 0.85f, 0.16f);
		DrawCuboidPolygon(headBaseRadius * 0.10f, headBaseRadius * 0.26f, headBaseRadius * 0.05f);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(i * headBaseRadius * 0.92f, headBaseRadius * 0.54f, -headBaseRadius * 0.18f);
		glRotatef((float)(-i * 22), 0.0f, 0.0f, 1.0f);
		glScalef(0.18f, 0.75f, 0.14f);
		DrawCuboidPolygon(headBaseRadius * 0.08f, headBaseRadius * 0.22f, headBaseRadius * 0.05f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.76f, 0.62f, 0.24f });
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, headBaseRadius * 1.02f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.09f, headBaseRadius * 0.09f, headBaseRadius * 0.02f, 0.28f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWoodElementClothAccents(float torsoRadius, float torsoHeight, bool isChibi)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	ApplyTint({ 0.42f, 0.76f, 0.34f });

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 0.92f, -torsoHeight * 1.22f, torsoRadius * 1.02f);
		glRotatef((float)(i * 14), 0.0f, 0.0f, 1.0f);
		glScalef(0.26f, isChibi ? 0.95f : 1.25f, 0.12f);
		DrawCuboidPolygon(torsoRadius * 0.10f, torsoHeight * 0.75f, torsoRadius * 0.06f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, woodTexture);
	ApplyTint({ 0.58f, 0.42f, 0.24f });

	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 0.76f, torsoRadius * 1.04f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.28f, torsoRadius * 0.28f, torsoRadius * 0.04f, 0.20f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterEffectCloud(float x, float y, float z, float scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);

	glPushMatrix();
	glScalef(1.5f, 0.8f, 0.8f);
	DrawSphere(quadric, 0.12f, SLICES, STACKS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.12f, 0.04f, 0.0f);
	DrawSphere(quadric, 0.10f, SLICES, STACKS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.08f, 0.0f);
	DrawSphere(quadric, 0.12f, SLICES, STACKS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.13f, 0.03f, 0.0f);
	DrawSphere(quadric, 0.09f, SLICES, STACKS);
	glPopMatrix();

	glPopMatrix();
}

void DrawWaterRainDrop(float x, float y, float z, float scale, float tilt)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(tilt, 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, scale);

	glColor4f(0.58f, 0.82f, 0.95f, 0.90f);

	glPushMatrix();
	glTranslatef(0.0f, -0.030f, 0.0f);
	glScalef(0.70f, 1.0f, 0.70f);
	DrawSphere(quadric, 0.026f, 14, 14);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.020f, 0.0f);
	DrawCylinder(quadric, 0.020f, 0.001f, 0.065f, 14, 8);
	glPopMatrix();

	glColor4f(0.86f, 0.96f, 1.0f, 0.55f);
	glPushMatrix();
	glTranslatef(0.008f, -0.022f, 0.014f);
	glScalef(0.30f, 0.42f, 0.22f);
	DrawSphere(quadric, 0.020f, 10, 10);
	glPopMatrix();

	glPopMatrix();
}

void DrawWaterElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != WATER_ELEMENT)
		return;

	float currentTime = (float)GetTickCount() * 0.001f;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0.88f, 0.94f, 1.0f, 0.9f);
	DrawWaterEffectCloud(-0.42f, 0.36f, -1.95f, 1.15f);
	DrawWaterEffectCloud(0.05f, 0.46f, -2.05f, 1.35f);
	DrawWaterEffectCloud(0.48f, 0.32f, -1.9f, 1.05f);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	for (int i = 0; i < 42; i++)
	{
		float column = -0.78f + (float)(i % 14) * 0.12f;
		float layer = (float)(i / 14) * 0.26f;
		float speed = 0.75f + (float)((i * 7) % 9) * 0.09f;
		float phase = (float)((i * 13) % 17) * 0.06f;
		float sway = (float)((i * 5) % 7) * 0.006f;
		float dropY = 0.26f - fmodf(currentTime * speed + phase, 1.12f);
		float dropX = column + layer * 0.025f + sinf(currentTime * (0.6f + sway) + phase) * 0.01f;
		float dropZ = -1.72f - layer * 0.12f - (float)(i % 3) * 0.03f;
		float dropScale = 0.42f + (float)((i * 11) % 5) * 0.035f;
		float dropTilt = -10.0f - (float)((i * 3) % 6) * 2.0f;

		DrawWaterRainDrop(dropX, dropY, dropZ, dropScale, dropTilt);
	}
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	ResetMaterial();
}

void DrawForestTree(float x, float y, float z, float scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);

	glDisable(GL_TEXTURE_2D);
	glColor3f(0.42f, 0.26f, 0.14f);

	glPushMatrix();
	glTranslatef(0.0f, -0.10f, 0.0f);
	DrawCylinder(quadric, 0.038f, 0.030f, 0.34f, 14, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.035f, 0.00f, 0.0f);
	glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, 0.015f, 0.008f, 0.13f, 10, 6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.042f, 0.03f, 0.0f);
	glRotatef(32.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, 0.016f, 0.007f, 0.14f, 10, 6);
	glPopMatrix();

	glColor3f(0.24f, 0.56f, 0.20f);

	glPushMatrix();
	glTranslatef(0.0f, 0.13f, 0.0f);
	glScalef(1.0f, 1.15f, 1.0f);
	DrawSphere(quadric, 0.14f, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.10f, 0.06f, 0.0f);
	DrawSphere(quadric, 0.10f, 18, 18);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.11f, 0.07f, 0.0f);
	DrawSphere(quadric, 0.095f, 18, 18);
	glPopMatrix();

	glColor3f(0.34f, 0.70f, 0.28f);
	glPushMatrix();
	glTranslatef(-0.02f, 0.22f, 0.0f);
	DrawSphere(quadric, 0.09f, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.07f, 0.18f, 0.03f);
	DrawSphere(quadric, 0.07f, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.08f, 0.17f, -0.02f);
	DrawSphere(quadric, 0.075f, 16, 16);
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void DrawWoodElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != WOOD_ELEMENT)
		return;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, grassTexture);
	ApplyTint({ 0.42f, 0.74f, 0.34f });
	glPushMatrix();
	glTranslatef(0.0f, -0.56f, -2.15f);
	glScalef(1.0f, 0.55f, 0.18f);
	DrawCuboidPolygon(1.9f, 0.42f, 0.22f);
	glPopMatrix();

	DrawForestTree(-0.62f, -0.24f, -2.05f, 1.0f);
	DrawForestTree(-0.22f, -0.24f, -1.92f, 1.2f);
	DrawForestTree(0.22f, -0.24f, -2.00f, 1.05f);
	DrawForestTree(0.63f, -0.24f, -2.10f, 0.95f);
	DrawForestTree(-0.42f, -0.28f, -1.78f, 0.82f);
	DrawForestTree(0.42f, -0.28f, -1.82f, 0.86f);

	ResetTint();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	ResetMaterial();
}

void DrawCharacterHeadAccessory(float headBaseRadius)
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		DrawWaterElementHeadJewel(headBaseRadius);
		break;

	case METAL_ELEMENT:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, goldTexture);

		glPushMatrix();
		glTranslatef(headBaseRadius * 0.72f, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.82f, headBaseRadius * 0.82f, headBaseRadius * 0.05f, 0.12f, SLICES, STACKS, LOOPS);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		break;

	case WOOD_ELEMENT:
		DrawWoodElementHeadAccessory(headBaseRadius);
		break;
	}
}

void DrawCharacterBodyDecorations(float torsoRadius, float torsoHeight, bool isChibi)
{
	switch (currentCharacterTexturePresetIndex)
	{
	case FIRE_ELEMENT:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, redClothTexture);

		for (int i = -1; i <= 1; i += 2)
		{
			glPushMatrix();
			glTranslatef(0.0f, torsoHeight * 0.3f, i * torsoRadius * 0.75f);
			glRotatef((float)(i * 28), 1.0f, 0.0f, 0.0f);
			glScalef(0.9f, isChibi ? 0.75f : 1.0f, 0.18f);
			DrawCuboidPolygon(torsoRadius * 0.2f, torsoHeight * 1.35f, torsoRadius * 0.08f);
			glPopMatrix();
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, goldTexture);

		glPushMatrix();
		glTranslatef(0.0f, torsoHeight * 0.2f, torsoRadius * 1.08f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.22f, torsoRadius * 0.22f, torsoRadius * 0.08f, 0.35f, SLICES, STACKS, LOOPS);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		break;

	case WATER_ELEMENT:
		DrawWaterElementDecorations(torsoRadius, torsoHeight);
		break;

	case WOOD_ELEMENT:
		DrawWoodElementDecorations(torsoRadius, torsoHeight, isChibi);
		break;

	case METAL_ELEMENT:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, steelTexture);

		glPushMatrix();
		glTranslatef(0.0f, torsoHeight * 0.65f, torsoRadius * 0.15f);
		glPushMatrix();
		glTranslatef(-torsoRadius * 1.2f, 0.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
		DrawSemiSphere(quadric, torsoRadius * 0.36f, SLICES, STACKS);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(torsoRadius * 1.2f, 0.0f, 0.0f);
		glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
		DrawSemiSphere(quadric, torsoRadius * 0.36f, SLICES, STACKS);
		glPopMatrix();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, beltTexture);

		glPushMatrix();
		glTranslatef(0.0f, -torsoHeight * 2.0f, 0.0f);
		DrawBelt(torsoRadius);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		break;

	case EARTH_ELEMENT:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		glPushMatrix();
		glTranslatef(0.0f, -torsoHeight * 0.15f, -torsoRadius * 0.92f);
		glScalef(1.0f, 1.6f, 0.28f);
		DrawCuboidPolygon(torsoRadius * 1.1f, torsoHeight * 1.9f, torsoRadius * 0.2f);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, goldTexture);

		glPushMatrix();
		glTranslatef(0.0f, torsoHeight * 0.55f, torsoRadius * 1.03f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.55f, torsoRadius * 0.55f, torsoRadius * 0.09f, 0.18f, SLICES, STACKS, LOOPS);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, -torsoHeight * 1.6f, 0.0f);
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.72f, torsoRadius * 0.72f, torsoRadius * 0.08f, 0.12f, SLICES, STACKS, LOOPS);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, redClothTexture);

		for (int i = -1; i <= 1; i += 2)
		{
			glPushMatrix();
			glTranslatef(0.0f, -torsoHeight * 1.7f, i * torsoRadius * 0.45f);
			glRotatef((float)(i * 12), 1.0f, 0.0f, 0.0f);
			glScalef(0.9f, isChibi ? 0.8f : 1.0f, 0.18f);
			DrawCuboidPolygon(torsoRadius * 0.22f, torsoHeight * 1.2f, torsoRadius * 0.08f);
			glPopMatrix();
		}

		glDisable(GL_TEXTURE_2D);
		break;
	}
}

// ----------------------------------------------------

// ==============
// MODEL ASSEMBLY
// ==============

void DrawCharacter()
{
	bool isChibi = (currentCharacterMode == CHIBI_MODE);

	// Torso
	float torsoRadius = isChibi ? 0.05f : 0.06f;
	float torsoHeight = isChibi ? 0.024f : 0.03f;
	glPushMatrix();

	// Upper Torso
	glPushMatrix();
	PartRotation &upperTorso = parts[UPPER_TORSO];
	glRotatef(upperTorso.angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(upperTorso.angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(upperTorso.angleZ, 0.0f, 0.0f, 1.0f);
	DrawTorsoPart(torsoRadius, torsoHeight);

	// Red Vest [COSTUME]
	glPushMatrix();
	ApplyTint(GetElementClothTint());
	DrawVest(torsoRadius, torsoHeight);
	ResetTint();
	glPopMatrix();
	// END Red Vest [COSTUME]

	glPushMatrix();
	ApplyTint(GetElementAccentTint());
	DrawCharacterBodyDecorations(torsoRadius, torsoHeight, isChibi);
	ResetTint();
	glPopMatrix();

	if (currentCharacterTexturePresetIndex == WOOD_ELEMENT)
	{
		glPushMatrix();
		DrawWoodElementClothAccents(torsoRadius, torsoHeight, isChibi);
		glPopMatrix();
	}

	// Neck
	float neckRadius = 0.02f;
	float neckHeight = 0.065f;
	glPushMatrix();
	glTranslatef(0.0f, (torsoHeight / 2 + neckHeight * 1.1f), 0.0f);
	DrawNeck(neckRadius, neckHeight);

	//// Neck Ring
	// glPushMatrix();
	// glTranslatef(0.0f, neckHeight * 0.05f, 0.0f);
	// DrawNeckRing(neckRadius);
	// glPopMatrix();
	//// END Neck Ring

	// Head
	float headBaseRadius = isChibi ? 0.13f : 0.1f;
	float headBaseHeight = isChibi ? 0.065f : 0.05f;
	glPushMatrix();
	glTranslatef(0.0f, neckHeight + headBaseHeight * (isChibi ? 0.95f : 0.8f), 0.0f);
	PartRotation &head = parts[HEAD];
	glRotatef(head.angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(head.angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(head.angleZ, 0.0f, 0.0f, 1.0f);
	DrawHead(headBaseRadius, headBaseHeight);

	// Hair
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.5f, 0.0f);
	ApplyTint(GetElementHairTint());
	DrawHair(headBaseRadius);
	ResetTint();

	if (currentCharacterTexturePresetIndex == WOOD_ELEMENT)
	{
		glPushMatrix();
		DrawWoodElementHairDecorations(headBaseRadius);
		glPopMatrix();
	}

	// Hair Ribbon
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius, 0.0f);
	ApplyTint(GetElementClothTint());
	DrawHairRibbons(headBaseRadius);
	ResetTint();
	glPopMatrix();
	// END Hair Ribbon

	glPopMatrix();
	// END Hair

	glPushMatrix();
	ApplyTint(GetElementAccentTint());
	DrawCharacterHeadAccessory(headBaseRadius);
	ResetTint();
	glPopMatrix();

	glPopMatrix();
	// END Head

	glPopMatrix();
	// END Neck

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Arms
	glPushMatrix();
	glTranslatef(0.0f, torsoHeight, 0.0f);

	float torsoArmsOffsetX = isChibi ? 0.075f : 0.095f;

	// Left Arm
	glPushMatrix();
	glTranslatef(-torsoArmsOffsetX, 0.0f, 0.0f);
	DrawArm(-1.0f);
	glPopMatrix();
	// END Left Arm

	// Right Arm
	glPushMatrix();
	glTranslatef(torsoArmsOffsetX, 0.0f, 0.0f);

	DrawArm(1.0f);
	glPopMatrix();
	// END Right Arm

	glPopMatrix();
	// END Arms

	glPopMatrix();
	// END Upper Torso

	glDisable(GL_TEXTURE_2D);

	// --------------------

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Lower Torso
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight, 0.0f);
	PartRotation &lowerTorso = parts[LOWER_TORSO];
	glRotatef(lowerTorso.angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(lowerTorso.angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(lowerTorso.angleZ, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, -1.0f, 1.0f);
	DrawTorsoPart(torsoRadius, torsoHeight);

	glDisable(GL_TEXTURE_2D);

	glScalef(1.0f, -1.0f, 1.0f);

	// Gold Plate
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);

	glPushMatrix();
	glTranslatef(0.0f, -torsoRadius * 1.2f, torsoRadius * 0.8f);
	ApplyTint(GetElementAccentTint());
	DrawGoldPlates(torsoRadius);
	ResetTint();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	// END Gold Plate

	float sideScarfLength = 0.38f;
	float sideScarfAmplitude = 0.05f;
	float sideScarfFrequency = 30.0f;
	float sideScarfPhase = -5.0f;
	float sideScarfThickness = 0.02f;
	float sideScarfOffset = 0.0f;
	int sideScarfSegments = 40;

	// Left Scarf
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -torsoRadius);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	DrawSideScarf(
		sideScarfLength,	// length
		sideScarfAmplitude, // amplitude (curl strength)
		sideScarfFrequency, // frequency (tight curls)
		sideScarfPhase,		// phase
		sideScarfThickness, // thickness
		sideScarfOffset,	// offset
		sideScarfSegments	// segments
	);

	glPopMatrix();
	// END Left Scarf

	// Right Scarf
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -torsoRadius);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	DrawSideScarf(
		sideScarfLength,	// length
		sideScarfAmplitude, // amplitude (curl strength)
		sideScarfFrequency, // frequency (tight curls)
		sideScarfPhase,		// phase
		sideScarfThickness, // thickness
		sideScarfOffset,	// offset
		sideScarfSegments	// segments
	);
	glPopMatrix();
	// END Right Scarf

	// Upper Pant
	float upperPantRadius = 0.034f;
	float upperPantHeight = torsoRadius * 2.5f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	// Waistband
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 2.5f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, upperPantRadius, upperPantRadius, upperPantHeight, SLICES, STACKS);
	glPopMatrix();
	// END Waistband

	glDisable(GL_TEXTURE_2D);
	// END Upper Pant

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Legs
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 2.5f, 0.0f);

	float torsoLegOffsetX = isChibi ? 0.032f : 0.04f;

	// Left Leg
	glPushMatrix();
	glTranslatef(-torsoLegOffsetX, 0.0f, 0.0f);
	DrawLeg(-1.0f, torsoRadius);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	//// Pant Leg [LEFT]
	// glPushMatrix();
	// glTranslatef(0.0f, 0.0f, -torsoRadius * 0.65f);
	// glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	// glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	// DrawPantLeg(-1.0f, torsoRadius);
	// glPopMatrix();
	//// END Pant Leg [LEFT]

	glPopMatrix();
	// END Left Leg

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	// Right Leg
	glPushMatrix();
	glTranslatef(torsoLegOffsetX, 0.0f, 0.0f);
	// glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
	// glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	DrawLeg(1.0f, torsoRadius);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	//// Pant Leg [RIGHT]
	// glPushMatrix();
	// glTranslatef(0.0f, 0.0f, -torsoRadius * 0.65f);
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	// glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// DrawPantLeg(1.0f, torsoRadius);
	// glPopMatrix();
	//// END Pant Leg [RIGHT]

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	// END Right Leg

	glPopMatrix();
	// END Legs

	//// Belt
	// glPushMatrix();
	// glTranslatef(0.0f, -torsoHeight * 2.0f, 0.0f);
	// DrawBelt(torsoRadius);
	// glPopMatrix();
	//// END Belt

	glPopMatrix();
	// END Lower Torso

	glPopMatrix();
	// END Torso
}

// ------------------
// ENVIRONMENT SETUP
// ------------------

void DrawSky(GLUquadricObj *quadric, float radius, int slices, int stacks)
{
	glPushMatrix();

	// Optional: keep the sky fixed relative to camera
	glTranslatef(cameraX, cameraY, cameraZ);

	// Invert normals for inside view
	gluQuadricOrientation(quadric, GLU_INSIDE);
	glColor3f(0.53f, 0.81f, 0.92f); // light blue sky color

	gluSphere(quadric, radius, slices, stacks);

	glPopMatrix();
}

void DrawSea(float width, float depth, float y)
{
	glPushMatrix();
	glTranslatef(0.0f, y, 0.0f);

	glEnable(GL_COLOR_MATERIAL);

	glBindTexture(GL_TEXTURE_2D, seaTexture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	float w = width / 2.0f;
	float d = depth / 2.0f;

	glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal for lighting

	float repeat = 30.0f; // Texture repeat factor

	glTexCoord2f(0.0f, repeat);
	glVertex3f(-w, 0.0f, -d);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-w, 0.0f, d);
	glTexCoord2f(repeat, 0.0f);
	glVertex3f(w, 0.0f, d);
	glTexCoord2f(repeat, repeat);
	glVertex3f(w, 0.0f, -d);
	glEnd();

	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();
}



// -------------------------------------------------------

void Display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------

	// Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	SetupCamera();
	SetupLighting();
	ResetMaterial();

	DrawLightIndicator();
	DrawWaterElementBackgroundEffect();
	DrawWoodElementBackgroundEffect();

	// ZhaLing
	characterX = 0.0f;
	characterY = -0.13f;
	characterZ = -1.0f;
	glPushMatrix();
	glTranslatef(characterX, characterY, characterZ);
	DrawCharacter();
	glPopMatrix();
	// END ZhaLing

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
}

//--------------------------------------------------------------------

// Windows Entry Point (called by OS, the values are passed in by OS)
// HINSTANCE = Instance number or index number (ID No) of windows app or form (just like map() we need to assign key with index for each child components)
// The 2nd HInstance is the ID for the parent window (just like ActiveX) (obsolete parameters) (When we run, we need a parent window to keep track of the child window)
// LPSTR = Long Pointer to a string (USEFUL) (like we can issue a command before the window runs)
// e.g. In CMD:
//		ping.exe www.google.com (it will ping to this website)
//
//		OR
//			"C:\Program Files\Google\Chrome\Application\chrome.exe"
//			we can right as
//			"C:\Program Files\Google\Chrome\Application\chrome.exe" --incognito
//
// nCmdShow: OS will pass in (full screen, minimize, etc.)
int WINAPI WinMain(
	_In_ HINSTANCE hInst,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR cmdlparameter,
	_In_ int nCmdShow)
{
	// if (cmdlparameter == "debugmode")
	//{
	//	// do something (e.g. privilege)
	// }

	// WNDCLASSEX = Windows Class Extended (Data Structure) (We'll specify what we need)
	WNDCLASSEX wc;						 // data structure to store how you want the class to be
	ZeroMemory(&wc, sizeof(WNDCLASSEX)); // To fill up the memory with zeros based on the size of WNDCLASSEX to &wc

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL); // ask OS for the ID, same with passing the hInst
	wc.lpfnWndProc = WindowProcedure;	  // long pointer to a function
	wc.lpszClassName = CLASS_TITLE;		  // long pointer to a zero terminated string (e.g. "Hello World" actually is "Hello World0")
	wc.style = CS_HREDRAW | CS_VREDRAW;	  //

	if (!RegisterClassEx(&wc))
		return false; // We pass the wc to let the OS to build for us during runtime
	// (because we dont have the proprietary code for window instantiation)

	//	CW_USEDEFAULT = where should the window be displayed, if we put 0 and 0 is top left
	//	800x800 = width x height

	HWND hWnd = CreateWindow(CLASS_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
							 NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	// DC = Display context (pass it to handler like a pointer)
	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	InitPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	// to tell OS which instance we are specifying at (we may open many different window)
	if (!wglMakeCurrent(hdc, hglrc))
		return false;

	// Initialize textures used
	InitTextures();

	// Initialize parts
	InitParts();

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) // message loop
	{
		// PeekMessage = to ask OS any message to the window
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg); // SEND THIS MESSAGE TO WindowProcedure
		}

		// Setup Projection

		Display();

		SwapBuffers(hdc);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------
