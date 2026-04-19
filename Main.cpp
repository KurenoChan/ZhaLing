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
	CENTER,
	LSIDE,
	RSIDE,
	PERSPECTIVE,
	ORTHO,
	CAMERA_COUNT
};

CameraMode currentCameraMode = CENTER;

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

struct AnimationPose
{
	float torsoX = 0.0f, torsoY = 0.0f, lowerTorsoX = 0.0f, lowerTorsoY, headX = 0.0f;
	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f, rLowerArmX = 0.0f, rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;
	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f, lLowerArmX = 0.0f, lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;
	float lLegX = 0.0f, rLegX = 0.0f, lKneeX = 0.0f, rKneeX = 0.0f, lFootX = 0.0f, rFootX = 0.0f;
	float charZ = 0.0f, charY = 0.0f;
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
float characterY = -0.13f;
float characterZ = -1.0f;

float characterX_default = 0.0f;
float characterY_default = -0.13f;
float characterZ_default = -1.0f;

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
int currentRightWeapon = 0; // 0 = None, 1 = Spear, 2 = Sword (Add more later)
int currentLeftWeapon = 0; // 0 = Empty, 1 = Spear, 2 = Fish, 3 = Leaf, 4 = Gun
const int TOTAL_WEAPONS = 12; // Update this number as you add more weapons
float wheelRotationAngle = 0.0f; // Tracks the current spin position
float wheelRotationSpeed = 0.0f; // Tracks how fast it is spinning
int currentBladeIndex = 0;
const int TOTAL_BLADE_TEXTURES = 2; // 0 = Normal Blade, 1 = Red Blade
bool isGunShooting = false;
int shootTimer = 0;
// =========================
// Weapon ANIMATION VARIABLES
// =========================
int currentAnimType = 0;
const int TOTAL_ANIMS = 6; // We will just have 1 (Spear Thrust) for now
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
GLfloat light1Ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };	 // ambient = minimum brightness of scene
GLfloat light1Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	 // diffuse = real lighting that reveals geometry
GLfloat light1Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // specular = how glossy the surface looks
Color light1Color = { 0.8f, 0.8f, 0.8f };

// LIGHT 2: Warm Sunset / Golden Hour
GLfloat light2Ambient[] = { 0.2f, 0.1f, 0.1f, 1.0f };	 // Deep reddish shadows
GLfloat light2Diffuse[] = { 1.0f, 0.7f, 0.3f, 1.0f };	 // Strong orange/gold light
GLfloat light2Specular[] = { 1.0f, 0.9f, 0.7f, 1.0f }; // Bright yellow-white glints
Color light2Color = { 1.0f, 0.7f, 0.3f };

// LIGHT 3: Cold Moonlight / Cyberpunk
GLfloat light3Ambient[] = { 0.1f, 0.1f, 0.2f, 1.0f };	 // Faint blue ambient
GLfloat light3Diffuse[] = { 0.4f, 0.6f, 1.0f, 1.0f };	 // Cool blue-white light
GLfloat light3Specular[] = { 0.8f, 0.8f, 1.0f, 1.0f }; // Sharp icy highlights
Color light3Color = { 0.4f, 0.6f, 1.0f };

// Collection of lights for easy looping
const GLfloat* lightsAmbient[] = { light1Ambient, light2Ambient, light3Ambient };
const GLfloat* lightsDiffuse[] = { light1Diffuse, light2Diffuse, light3Diffuse };
const GLfloat* lightsSpecular[] = { light1Specular, light2Specular, light3Specular };
const Color* lightsColor[] = { &light1Color, &light2Color, &light3Color };

const int NUM_LIGHTS = sizeof(lightsAmbient) / sizeof(lightsAmbient[0]);
int lightIndex = 0;

// -------------------
// Material Setup
// -------------------
GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat defaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat defaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat defaultShininess = 0.0f;

// Gold Plate
GLfloat gold_matAmbient[] = { 0.247f, 0.199f, 0.074f, 1.0f };
GLfloat gold_matDiffuse[] = { 0.751f, 0.606f, 0.226f, 1.0f };
GLfloat gold_matSpecular[] = { 0.628f, 0.556f, 0.366f, 1.0f };
GLfloat gold_shininess = 51.2f;

// Gray Silk Scarf
GLfloat silk_matAmbient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat silk_matDiffuse[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat silk_matSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat silk_shininess = 90.0f;

// Ground
GLfloat ground_matAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat ground_matDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat ground_matSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat ground_shininess = 90.0f;

// -------------------
// GLU Quadric Objects
// -------------------
GLUquadricObj* quadric = gluNewQuadric();
GLUquadric* skyQuadric = gluNewQuadric();

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
GLuint chainTexture;
GLuint dirtTexture;

// PROPS TEXTURES
GLuint beltTexture;

// Environment Textures
GLuint skyTexture;
GLuint seaTexture;
GLuint groundTexture;

// Weapon Textures
GLuint goldenTexture;
GLuint sliverTexture;
GLuint spearBlade;
GLuint spearRedBlade;
GLuint fireCoreTexture;
GLuint fireOuterTexture;
GLuint fireWheelBladeTexture;
GLuint fireWheelRing;
GLuint redCoreTexture;
GLuint blueFireCoreTexture;
GLuint blueFireOuterTexture;
GLuint fishFinTexture;
GLuint fishTailTexture;
GLuint fishBodyTexture;
GLuint fishUpperBackTexture;
GLuint fishEyeWhiteTexture;
GLuint fishGoldBackTexture;
GLuint bananaLeafTexture;
GLuint bananaLeafHandleTexture;
GLuint goldBananaLeafTexture;
GLuint blackSilverTexture;
GLuint diamondTexture;

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

	characterX = characterX_default;
	characterY = characterY_default;
	characterZ = characterZ_default;
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

	const CharacterTexturePreset& preset = characterTexturePresets[currentCharacterTexturePresetIndex];
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
	characterTexturePresets.push_back({ skinTexture, hairTexture, redBlackFlameTexture, redClothTexture, goldTexture });
	characterTexturePresets.push_back({ skinTexture, seaTexture, skyTexture, skyTexture, silverTexture });
	characterTexturePresets.push_back({ skinTexture, grassTexture, woodTexture, grassTexture, goldTexture });
	characterTexturePresets.push_back({ skinTexture, goldTexture, goldTexture, goldTexture, goldTexture });
	characterTexturePresets.push_back({ skinTexture, brownClothTexture, dirtTexture, dirtTexture, goldTexture });

	currentCharacterTexturePresetIndex = FIRE_ELEMENT;
	ApplyCharacterTexturePreset();
}

Color GetElementClothTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return { 0.15f, 0.28f, 0.42f };
	case WOOD_ELEMENT:
		return { 0.35f, 0.75f, 0.32f };
	case METAL_ELEMENT:
		return { 0.95f, 0.82f, 0.28f };
	case EARTH_ELEMENT:
		return { 0.55f, 0.36f, 0.20f };
	default:
		return { 1.0f, 1.0f, 1.0f };
	}
}

Color GetElementAccentTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return { 0.86f, 0.72f, 0.36f };
	case WOOD_ELEMENT:
		return { 0.55f, 0.88f, 0.45f };
	case METAL_ELEMENT:
		return { 1.0f, 0.86f, 0.35f };
	case EARTH_ELEMENT:
		return { 0.62f, 0.42f, 0.24f };
	default:
		return { 1.0f, 1.0f, 1.0f };
	}
}

Color GetElementHairTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return { 0.20f, 0.42f, 0.60f };
	case WOOD_ELEMENT:
		return { 0.42f, 0.74f, 0.34f };
	case METAL_ELEMENT:
		return { 0.86f, 0.86f, 0.90f };
	case EARTH_ELEMENT:
		return { 0.58f, 0.42f, 0.24f };
	default:
		return { 1.0f, 1.0f, 1.0f };
	}
}

Color GetElementScarfTint()
{
	switch (currentCharacterTexturePresetIndex)
	{
	case WATER_ELEMENT:
		return { 0.28f, 0.52f, 0.72f };
	case WOOD_ELEMENT:
		return { 0.36f, 0.68f, 0.30f };
	case METAL_ELEMENT:
		return { 0.82f, 0.78f, 0.42f };
	case EARTH_ELEMENT:
		return { 0.60f, 0.40f, 0.22f };
	default:
		return { 0.45f, 0.45f, 0.45f };
	}
}

void ApplyTint(const Color& color)
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

// ---------
// Animation
// ---------

void WalkAnimation() {
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	ResetModel();

	// ==========================================
	// FORCE UNEQUIP WEAPON
	// This tells DrawArm to use 'case 0' (No Weapon)
	// ==========================================
	currentLeftWeapon = 0;
	currentRightWeapon = 0;
	isGunShooting = false; // Safety: make sure flash is off
	isLooping = true; // Force walk cycles to loop!
	animFrame += (1.0f * animSpeed);

	// We split the animation into two parts:
	float windUpFrames = 30.0f; // Takes 30 frames to transition from 0.0f to the Walk Pose
	float walkFrames = 60.0f;   // Takes 60 frames to do one full walk cycle
	float maxFrames = windUpFrames + walkFrames; // Total = 90.0f

	if (animFrame > maxFrames) {
		// MAGIC TRICK: Instead of resetting to 0.0f, we reset to the start of the WALK loop!
		// This skips the wind-up so the character keeps walking smoothly.
		animFrame = windUpFrames;
	}

	// ==========================================
	// 1. DECLARE ALL VARIABLES (Set to 0.0f)
	// ==========================================
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;

	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;

	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;

	float lLegX = 0.0f, lLegY = 0.0f, lLegZ = 0.0f;
	float lKneeX = 0.0f;
	float lFootX = 0.0f, lFootZ = 0.0f;

	float rLegX = 0.0f, rLegY = 0.0f, rLegZ = 0.0f;
	float rKneeX = 0.0f;
	float rFootX = 0.0f, rFootZ = 0.0f;

	float charX = characterX, charY = characterY, charZ = characterZ;

	// ==========================================
	// 2. WALK PHASES
	// ==========================================

	// ---------------------------------------------------------
	// PHASE 1: Wind-up (Transition from Initial Motion to Walk Pose)
	// ---------------------------------------------------------
	if (animFrame <= windUpFrames) {
		float phaseT = animFrame / windUpFrames;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// Gradually move the arms into your custom Walk Base Pose
		lArmX = ease * 45.0f; lArmY = ease * 85.0f;
		lLowerArmX = ease * -85.0f; lLowerArmY = ease * 75.0f;
		lHandY = ease * 15.0f;

		rArmX = ease * -25.0f; rArmY = ease * 75.0f;
		rLowerArmX = ease * 65.0f; rLowerArmY = ease * 80.0f;
		rHandX = ease * 25.0f; rHandY = ease * 10.0f;

		// (Notice we leave legs at 0.0f here so the character starts from a standing position!)
	}

	// ---------------------------------------------------------
	// PHASE 2: The Looping Walk Cycle
	// ---------------------------------------------------------
	else {
		// Create a specific 't' just for the walk cycle (goes from 0.0 to 1.0 during Phase 2)
		float walkT = (animFrame - windUpFrames) / walkFrames;
		float cycle = walkT * 2.0f * 3.14159f;

		// 1. Lock in the Base Poses (100% applied)
		lArmX = 45.0f; lArmY = 85.0f; lArmZ = 0.0f;
		lLowerArmX = -85.0f; lLowerArmY = 75.0f;
		lHandX = 0.0f; lHandY = 15.0f; lHandZ = 0.0f;

		rArmX = -25.0f; rArmY = 75.0f; rArmZ = 0.0f;
		rLowerArmX = 65.0f; rLowerArmY = 80.0f;
		rHandX = 25.0f; rHandY = 10.0f; rHandZ = 0.0f;

		// 2. Calculate the "Seesaw" swing
		float armSwing = sin(cycle) * 35.0f;
		float legSwing = sin(cycle) * 35.0f;

		// 3. Apply the swings to the base poses
		lArmX += armSwing;
		rArmX += armSwing; // (Both use += because of the mirrored model axes!)

		lLegX -= legSwing;
		rLegX += legSwing;

		if (lLegX > 0) lKneeX = lLegX;
		if (rLegX > 0) rKneeX = rLegX;

		charY = characterY + abs(sin(cycle)) * 0.01f;
	}

	// ==========================================
	// 3. APPLY VARIABLES TO PARTS
	// ==========================================
	parts[HEAD].angleX = headX; parts[HEAD].angleY = headY; parts[HEAD].angleZ = headZ;

	parts[UPPER_TORSO].angleX = uTorsoX; parts[UPPER_TORSO].angleY = uTorsoY; parts[UPPER_TORSO].angleZ = uTorsoZ;
	parts[LOWER_TORSO].angleX = lTorsoX; parts[LOWER_TORSO].angleY = lTorsoY; parts[LOWER_TORSO].angleZ = lTorsoZ;

	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY; parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleX = lHandX; parts[LEFT_HAND].angleY = lHandY; parts[LEFT_HAND].angleZ = lHandZ;

	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY; parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY; parts[RIGHT_HAND].angleZ = rHandZ;

	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_UPPER_LEG].angleY = lLegY; parts[LEFT_UPPER_LEG].angleZ = lLegZ;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX; parts[LEFT_FOOT].angleZ = lFootZ;

	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_UPPER_LEG].angleY = rLegY; parts[RIGHT_UPPER_LEG].angleZ = rLegZ;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX; parts[RIGHT_FOOT].angleZ = rFootZ;

	characterX = charX; characterY = charY; characterZ = charZ;
}

void SpearAttack() {
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 1;
	float maxFrames = 90.0f;
	animFrame += (1.0f * animSpeed);

	// Handle Looping logic
	if (animFrame > maxFrames)
	{
		if (isLooping)
		{
			animFrame = 0.0f; // Restart
		}
		else
		{
			animFrame = maxFrames; // Lock it at the last frame
			isPlaying = false;	   // Auto-pause at the end
		}
	}

	float t = animFrame / maxFrames;

	// ==========================================
	// 1. DECLARE ALL VARIABLES (Set to 0)
	// ==========================================
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;

	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;

	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;

	float lLegX = 0.0f, lLegY = 0.0f, lLegZ = 0.0f;
	float lKneeX = 0.0f;
	float lFootX = 0.0f, lFootZ = 0.0f;

	float rLegX = 0.0f, rLegY = 0.0f, rLegZ = 0.0f;
	float rKneeX = 0.0f;
	float rFootX = 0.0f, rFootZ = 0.0f;

	float charX = characterX, charY = characterY, charZ = characterZ;

	// ==========================================
	// 2. ANIMATION PHASES
	// ==========================================
		// ---------------------------------------------------------
		// PHASE 1: Wind-up (0% to 40%) 
		// Formula: ease * TARGET_1
		// ---------------------------------------------------------
	if (t <= 0.4f) {
		float phaseT = t / 0.4f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = ease * 0.0f;
		headY = ease * -20.0f;
		headZ = ease * 0.0f;
		uTorsoX = ease * 0.0f;
		uTorsoY = ease * 15.0f;
		uTorsoZ = ease * 0.0f;
		lTorsoX = ease * 0.0f;
		lTorsoY = ease * 15.0f;
		lTorsoZ = ease * 0.0f;

		lArmX = ease * -60.0f; lArmY = ease * -30.0f; lArmZ = ease * 0.0f;
		lLowerArmX = ease * 70.0f; lLowerArmY = ease * 130.0f;
		lHandX = ease * -15.0f; lHandY = ease * 30.0f; lHandZ = ease * 0.0f;

		rArmX = ease * 35.0f; rArmY = ease * -30.0f; rArmZ = ease * 0.0f;
		rLowerArmX = ease * -5.0f; rLowerArmY = ease * 0.0f;
		rHandX = ease * 0.0f; rHandY = ease * 30.0f; rHandZ = ease * 0.0f;

		lLegX = ease * -50.0f; lLegY = ease * 10.0f; lLegZ = ease * 0.0f;
		lKneeX = ease * 40.0f;
		rLegX = ease * 0.0f; rLegY = ease * 20.0f; rLegZ = ease * 0.0f;
		rKneeX = ease * 50.0f;

		charX = characterX; charY = characterY; charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 2: Thrust (40% to 60%)
	// Formula: START + (ease * (TARGET - START))
	// ---------------------------------------------------------
	else if (t <= 0.6f) {
		float phaseT = (t - 0.4f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// HEAD
		headX = 0.0f + (ease * (0.0f - 0.0f));
		headY = -20.0f + (ease * (25.0f - (-20.0f)));
		headZ = 0.0f + (ease * (0.0f - 0.0f));

		// UPPER TORSO
		uTorsoX = 0.0f + (ease * (0.0f - 0.0f));
		uTorsoY = 15.0f + (ease * (-25.0f - 15.0f));
		uTorsoZ = 0.0f + (ease * (0.0f - 0.0f));

		// LOWER TORSO
		lTorsoX = 0.0f + (ease * (0.0f - 0.0f));
		lTorsoY = 15.0f + (ease * (-15.0f - 15.0f));
		lTorsoZ = 0.0f + (ease * (0.0f - 0.0f));

		// LEFT ARM (Updated with your new targets!)
		lArmX = -60.0f + (ease * (-55.0f - (-60.0f)));
		lArmY = -30.0f + (ease * (-25.0f - (-30.0f)));
		lArmZ = 0.0f + (ease * (0.0f - 0.0f));

		lLowerArmX = 70.0f + (ease * (35.0f - 70.0f));     // Target is 35
		lLowerArmY = 130.0f + (ease * (115.0f - 130.0f));  // Target is 115

		lHandX = -15.0f + (ease * (-100.0f - (-15.0f)));   // Target is -100
		lHandY = 30.0f + (ease * (0.0f - 30.0f));          // Target is 0
		lHandZ = 0.0f + (ease * (0.0f - 0.0f));

		// RIGHT ARM
		rArmX = 35.0f + (ease * (0.0f - 35.0f));
		rArmY = -30.0f + (ease * (65.0f - (-30.0f)));
		rArmZ = 0.0f + (ease * (0.0f - 0.0f));
		rLowerArmX = -5.0f + (ease * (0.0f - (-5.0f)));
		rLowerArmY = 0.0f + (ease * (5.0f - 0.0f));
		rHandX = 0.0f + (ease * (0.0f - 0.0f));
		rHandY = 30.0f + (ease * (-90.0f - 30.0f));
		rHandZ = 0.0f + (ease * (0.0f - 0.0f));

		// LEFT LEG
		lLegX = -50.0f + (ease * (35.0f - (-50.0f)));
		lLegY = 10.0f + (ease * (10.0f - 10.0f));
		lLegZ = 0.0f + (ease * (0.0f - 0.0f));
		lKneeX = 40.0f + (ease * (0.0f - 40.0f));

		// RIGHT LEG
		rLegX = 0.0f + (ease * (-45.0f - 0.0f));
		rLegY = 20.0f + (ease * (20.0f - 20.0f));
		rLegZ = 0.0f + (ease * (0.0f - 0.0f));
		rKneeX = 50.0f + (ease * (25.0f - 50.0f));

		charX = characterX, charY = characterY, charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 3: Hold Pose (60% to 80%)
	// ---------------------------------------------------------
	else if (t <= 0.8f) {
		headX = 0.0f; headY = 25.0f; headZ = 0.0f;
		uTorsoX = 0.0f; uTorsoY = -25.0f; uTorsoZ = 0.0f;
		lTorsoX = 0.0f; lTorsoY = -15.0f; lTorsoZ = 0.0f;

		// Left arm holds your new poses!
		lArmX = -55.0f; lArmY = -25.0f; lArmZ = 0.0f;
		lLowerArmX = 35.0f; lLowerArmY = 115.0f;
		lHandX = -100.0f; lHandY = 0.0f; lHandZ = 0.0f;

		rArmX = 0.0f; rArmY = 65.0f; rArmZ = 0.0f;
		rLowerArmX = 0.0f; rLowerArmY = 5.0f;
		rHandX = 0.0f; rHandY = -90.0f; rHandZ = 0.0f;

		lLegX = 35.0f; lLegY = 10.0f; lLegZ = 0.0f;
		lKneeX = 0.0f;

		rLegX = -45.0f; rLegY = 20.0f; rLegZ = 0.0f;
		rKneeX = 25.0f;

		charX = characterX, charY = characterY, charZ = characterZ;
	}

		// ---------------------------------------------------------
		// PHASE 4: Recovery (80% to 100%)
		// ---------------------------------------------------------
		else {
			float phaseT = (t - 0.8f) / 0.2f;
			float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

			headX = 0.0f - (ease * 0.0f);
			headY = 25.0f - (ease * 25.0f);
			headZ = 0.0f - (ease * 0.0f);

			uTorsoX = 0.0f - (ease * 0.0f);
			uTorsoY = -25.0f - (ease * -25.0f);
			uTorsoZ = 0.0f - (ease * 0.0f);

			lTorsoX = 0.0f - (ease * 0.0f);
			lTorsoY = -15.0f - (ease * -15.0f);
			lTorsoZ = 0.0f - (ease * 0.0f);

			// Left arm returns from your new poses!
			lArmX = -55.0f - (ease * -55.0f);
			lArmY = -25.0f - (ease * -25.0f);
			lArmZ = 0.0f - (ease * 0.0f);
			lLowerArmX = 35.0f - (ease * 35.0f);
			lLowerArmY = 115.0f - (ease * 115.0f);
			lHandX = -100.0f - (ease * -100.0f);
			lHandY = 0.0f - (ease * 0.0f);
			lHandZ = 0.0f - (ease * 0.0f);

			rArmX = 0.0f - (ease * 0.0f);
			rArmY = 65.0f - (ease * 65.0f);
			rArmZ = 0.0f - (ease * 0.0f);
			rLowerArmX = 0.0f - (ease * 0.0f);
			rLowerArmY = 5.0f - (ease * 5.0f);
			rHandX = 0.0f - (ease * 0.0f);
			rHandY = -90.0f - (ease * -90.0f);
			rHandZ = 0.0f - (ease * 0.0f);

			lLegX = 35.0f - (ease * 35.0f);
			lLegY = 10.0f - (ease * 10.0f);
			lLegZ = 0.0f - (ease * 0.0f);
			lKneeX = 0.0f - (ease * 0.0f);

			rLegX = -45.0f - (ease * -45.0f);
			rLegY = 20.0f - (ease * 20.0f);
			rLegZ = 0.0f - (ease * 0.0f);
			rKneeX = 25.0f - (ease * 25.0f);

			charX = characterX, charY = characterY, charZ = characterZ;
		}

	// ==========================================
	// 3. APPLY VARIABLES TO PARTS
	// (This runs for every animation, outside the IF statement)
	// ==========================================
	parts[HEAD].angleX = headX; parts[HEAD].angleY = headY; parts[HEAD].angleZ = headZ;

	parts[UPPER_TORSO].angleX = uTorsoX; parts[UPPER_TORSO].angleY = uTorsoY; parts[UPPER_TORSO].angleZ = uTorsoZ;
	parts[LOWER_TORSO].angleX = lTorsoX; parts[LOWER_TORSO].angleY = lTorsoY; parts[LOWER_TORSO].angleZ = lTorsoZ;

	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY; parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleX = lHandX; parts[LEFT_HAND].angleY = lHandY; parts[LEFT_HAND].angleZ = lHandZ;

	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY; parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY; parts[RIGHT_HAND].angleZ = rHandZ;

	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_UPPER_LEG].angleY = lLegY; parts[LEFT_UPPER_LEG].angleZ = lLegZ;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX; parts[LEFT_FOOT].angleZ = lFootZ;

	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_UPPER_LEG].angleY = rLegY; parts[RIGHT_UPPER_LEG].angleZ = rLegZ;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX; parts[RIGHT_FOOT].angleZ = rFootZ;

	characterX = charX;
	characterY = charY;
	characterZ = charZ;
}

void SlashAnimation() {
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 3;
	animFrame += (1.0f * animSpeed);
	float maxFrames = 90.0f;

	if (animFrame > maxFrames) {
		if (isLooping) animFrame = 0.0f;
		else {
			animFrame = maxFrames;
			isPlaying = false;
		}
	}

	float t = animFrame / maxFrames;

	// ==========================================
	// 1. DECLARE ALL VARIABLES (Set to 0)
	// ==========================================
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;

	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;

	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;

	float lLegX = 0.0f, lLegY = 0.0f, lLegZ = 0.0f;
	float lKneeX = 0.0f;
	float lFootX = 0.0f, lFootZ = 0.0f;

	float rLegX = 0.0f, rLegY = 0.0f, rLegZ = 0.0f;
	float rKneeX = 0.0f;
	float rFootX = 0.0f, rFootZ = 0.0f;

	float charX = characterX, charY = characterY, charZ = characterZ;

	// ==========================================
	// 2. ANIMATION PHASES
	// ==========================================

	// ---------------------------------------------------------
	// PHASE 1: Wind-up (0% to 40%) 
	// (Your exact poses! Raising the heavy sword)
	// ---------------------------------------------------------
	if (t <= 0.4f) {
		float phaseT = t / 0.4f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = ease * 0.0f;
		headY = ease * -40.0f;
		headZ = ease * 0.0f;

		uTorsoX = ease * 0.0f;
		uTorsoY = ease * 40.0f;
		uTorsoZ = ease * 0.0f;

		lTorsoX = ease * 0.0f;
		lTorsoY = ease * 25.0f;
		lTorsoZ = ease * 0.0f;

		lArmX = ease * 50.0f; lArmY = ease * 50.0f; lArmZ = ease * 0.0f;
		lLowerArmX = ease * -75.0f; lLowerArmY = ease * 85.0f;
		lHandX = ease * -85.0f; lHandY = ease * 5.0f; lHandZ = ease * 0.0f;

		rArmX = ease * 80.0f; rArmY = ease * 0.0f; rArmZ = ease * 0.0f;
		rLowerArmX = ease * 0.0f; rLowerArmY = ease * 0.0f;
		rHandX = ease * 10.0f; rHandY = ease * -45.0f; rHandZ = ease * 0.0f;

		lLegX = ease * -25.0f; lLegY = ease * 20.0f; lLegZ = ease * 0.0f;
		lKneeX = ease * 20.0f;
		lFootX = ease * 0.0f; lFootZ = ease * 0.0f;

		rLegX = ease * 10.0f; rLegY = ease * 20.0f; rLegZ = ease * 0.0f;
		rKneeX = ease * 15.0f;
		rFootX = ease * -5.0f; rFootZ = ease * 0.0f;

		charX = characterX, charY = characterY, charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 2: Heavy Slash Impact (40% to 60%)
	// (Swinging down in a huge arc across the body!)
	// ---------------------------------------------------------
	else if (t <= 0.6f) {
		float phaseT = (t - 0.4f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// Head follows the swing
		headX = 0.0f + (ease * (10.0f - 0.0f));
		headY = -40.0f + (ease * (30.0f - (-40.0f)));
		headZ = 0.0f;

		// Torso violently twists the OTHER way to generate power!
		uTorsoX = 0.0f + (ease * (15.0f - 0.0f));   // Lean forward slightly
		uTorsoY = 40.0f + (ease * (-45.0f - 40.0f)); // Twist left
		uTorsoZ = 0.0f;

		lTorsoX = 0.0f + (ease * (10.0f - 0.0f));
		lTorsoY = 25.0f + (ease * (-25.0f - 25.0f));
		lTorsoZ = 0.0f;

		// Left arm drops and gets out of the way
		lArmX = 50.0f + (ease * (-30.0f - 50.0f));
		lArmY = 50.0f + (ease * (-20.0f - 50.0f));
		lArmZ = 0.0f;
		lLowerArmX = -75.0f + (ease * (-10.0f - (-75.0f)));
		lLowerArmY = 85.0f + (ease * (0.0f - 85.0f));
		lHandX = -85.0f + (ease * (0.0f - (-85.0f)));
		lHandY = 5.0f + (ease * (0.0f - 5.0f));
		lHandZ = 0.0f;

		// Right arm (The Sword Arm) swings down past the waist!
		rArmX = 80.0f + (ease * (-55.0f - 80.0f));     // Swing DOWN
		rArmY = 0.0f + (ease * (45.0f - 0.0f));        // Swing ACROSS the body
		rArmZ = 0.0f + (ease * (0.0f - 0.0f));
		rLowerArmX = 0.0f + (ease * (0.0f - 0.0f));    // Keep elbow straight for a wide arc
		rLowerArmY = 0.0f + (ease * (0.0f - 0.0f));
		rHandX = 10.0f + (ease * (35.0f - 10.0f));     // Snap the wrist forward for the hit
		rHandY = -45.0f + (ease * (0.0f - (-45.0f)));  // Untwist wrist
		rHandZ = 0.0f;

		// Legs brace for the impact (Left leg steps deeper)
		lLegX = -25.0f + (ease * (-35.0f - (-25.0f)));
		lLegY = 20.0f + (ease * (0.0f - 20.0f));
		lLegZ = 0.0f;
		lKneeX = 20.0f + (ease * (35.0f - 20.0f));
		lFootX = 0.0f; lFootZ = 0.0f;

		rLegX = 10.0f + (ease * (25.0f - 10.0f));     // Right leg stretches back
		rLegY = 20.0f + (ease * (0.0f - 20.0f));
		rLegZ = 0.0f;
		rKneeX = 15.0f + (ease * (0.0f - 15.0f));     // Right knee straightens
		rFootX = -5.0f + (ease * (-5.0f - (-5.0f)));
		rFootZ = 0.0f;

		charX = characterX; charY = characterY; charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 3: Hold Pose (60% to 80%)
	// (Hold the follow-through of the slash)
	// ---------------------------------------------------------
	else if (t <= 0.8f) {
		headX = 10.0f; headY = 30.0f; headZ = 0.0f;
		uTorsoX = 15.0f; uTorsoY = -45.0f; uTorsoZ = 0.0f;
		lTorsoX = 10.0f; lTorsoY = -25.0f; lTorsoZ = 0.0f;

		lArmX = -30.0f; lArmY = -20.0f; lArmZ = 0.0f;
		lLowerArmX = -10.0f; lLowerArmY = 0.0f;
		lHandX = 0.0f; lHandY = 0.0f; lHandZ = 0.0f;

		rArmX = -55.0f; rArmY = 45.0f; rArmZ = 0.0f;
		rLowerArmX = 0.0f; rLowerArmY = 0.0f;
		rHandX = 35.0f; rHandY = 0.0f; rHandZ = 0.0f;

		lLegX = -35.0f; lLegY = 0.0f; lLegZ = 0.0f;
		lKneeX = 35.0f;
		lFootX = 0.0f; lFootZ = 0.0f;

		rLegX = 25.0f; rLegY = 0.0f; rLegZ = 0.0f;
		rKneeX = 0.0f;
		rFootX = -5.0f; rFootZ = 0.0f;

		charX = characterX; charY = characterY; charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 4: Recovery (80% to 100%)
	// (Pulling the heavy sword back to standing position)
	// ---------------------------------------------------------
	else {
		float phaseT = (t - 0.8f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = 10.0f - (ease * 10.0f);
		headY = 30.0f - (ease * 30.0f);
		headZ = 0.0f - (ease * 0.0f);

		uTorsoX = 15.0f - (ease * 15.0f);
		uTorsoY = -45.0f - (ease * -45.0f);
		uTorsoZ = 0.0f - (ease * 0.0f);

		lTorsoX = 10.0f - (ease * 10.0f);
		lTorsoY = -25.0f - (ease * -25.0f);
		lTorsoZ = 0.0f - (ease * 0.0f);

		lArmX = -30.0f - (ease * -30.0f);
		lArmY = -20.0f - (ease * -20.0f);
		lArmZ = 0.0f - (ease * 0.0f);
		lLowerArmX = -10.0f - (ease * -10.0f);
		lLowerArmY = 0.0f - (ease * 0.0f);
		lHandX = 0.0f - (ease * 0.0f);
		lHandY = 0.0f - (ease * 0.0f);
		lHandZ = 0.0f - (ease * 0.0f);

		rArmX = -55.0f - (ease * -55.0f);
		rArmY = 45.0f - (ease * 45.0f);
		rArmZ = 0.0f - (ease * 0.0f);
		rLowerArmX = 0.0f - (ease * 0.0f);
		rLowerArmY = 0.0f - (ease * 0.0f);
		rHandX = 35.0f - (ease * 35.0f);
		rHandY = 0.0f - (ease * 0.0f);
		rHandZ = 0.0f - (ease * 0.0f);

		lLegX = -35.0f - (ease * -35.0f);
		lLegY = 0.0f - (ease * 0.0f);
		lLegZ = 0.0f - (ease * 0.0f);
		lKneeX = 35.0f - (ease * 35.0f);
		lFootX = 0.0f - (ease * 0.0f);
		lFootZ = 0.0f - (ease * 0.0f);

		rLegX = 25.0f - (ease * 25.0f);
		rLegY = 0.0f - (ease * 0.0f);
		rLegZ = 0.0f - (ease * 0.0f);
		rKneeX = 0.0f - (ease * 0.0f);
		rFootX = -5.0f - (ease * -5.0f);
		rFootZ = 0.0f - (ease * 0.0f);

		charX = characterX; charY = characterY; charZ = characterZ;
	}

	// ==========================================
	// 3. APPLY VARIABLES TO PARTS
	// ==========================================
	parts[HEAD].angleX = headX; parts[HEAD].angleY = headY; parts[HEAD].angleZ = headZ;

	parts[UPPER_TORSO].angleX = uTorsoX; parts[UPPER_TORSO].angleY = uTorsoY; parts[UPPER_TORSO].angleZ = uTorsoZ;
	parts[LOWER_TORSO].angleX = lTorsoX; parts[LOWER_TORSO].angleY = lTorsoY; parts[LOWER_TORSO].angleZ = lTorsoZ;

	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY; parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleX = lHandX; parts[LEFT_HAND].angleY = lHandY; parts[LEFT_HAND].angleZ = lHandZ;

	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY; parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY; parts[RIGHT_HAND].angleZ = rHandZ;

	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_UPPER_LEG].angleY = lLegY; parts[LEFT_UPPER_LEG].angleZ = lLegZ;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX; parts[LEFT_FOOT].angleZ = lFootZ;

	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_UPPER_LEG].angleY = rLegY; parts[RIGHT_UPPER_LEG].angleZ = rLegZ;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX; parts[RIGHT_FOOT].angleZ = rFootZ;

	characterX = charX; characterY = charY; characterZ = charZ;
}

void FirewheelAnimation() {
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	ResetModel();

	currentLeftWeapon = 0;
	currentRightWeapon = 7;

	animFrame += (1.0f * animSpeed);
	float maxFrames = 90.0f;

	if (animFrame > maxFrames) {
		if (isLooping) animFrame = 0.0f;
		else {
			animFrame = maxFrames;
			isPlaying = false;
		}
	}

	float t = animFrame / maxFrames;

	// ==========================================
	// 1. DECLARE ALL VARIABLES (Set to 0)
	// ==========================================
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;

	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;

	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;

	float lLegX = 0.0f, lLegY = 0.0f, lLegZ = 0.0f;
	float lKneeX = 0.0f;
	float lFootX = 0.0f, lFootZ = 0.0f;

	float rLegX = 0.0f, rLegY = 0.0f, rLegZ = 0.0f;
	float rKneeX = 0.0f;
	float rFootX = 0.0f, rFootZ = 0.0f;

	float baseY = characterY;

	float charX = characterX, charY = characterY, charZ = characterZ;

	// ==========================================
	// 2. ANIMATION PHASES
	// ==========================================

	// ---------------------------------------------------------
	// PHASE 1: Wind-up / Aiming (0% to 40%) 
	// Easing from 0.0f into your Phase 1 numbers!
	// ---------------------------------------------------------
	if (t <= 0.4f) {
		float phaseT = t / 0.4f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = ease * -10.0f; headY = 0.0f; headZ = 0.0f;

		uTorsoX = ease * 20.0f; uTorsoY = 0.0f; uTorsoZ = 0.0f;
		lTorsoX = ease * 15.0f; lTorsoY = 0.0f; lTorsoZ = 0.0f;

		lArmX = ease * 60.0f; lArmY = ease * 60.0f; lArmZ = 0.0f;
		lLowerArmX = ease * -55.0f; lLowerArmY = ease * 115.0f;
		lHandX = ease * -30.0f; lHandY = ease * 5.0f; lHandZ = 0.0f;

		rArmX = ease * 5.0f; rArmY = ease * -30.0f; rArmZ = 0.0f;
		rLowerArmX = ease * 35.0f; rLowerArmY = 0.0f;
		rHandX = ease * 20.0f; rHandY = ease * -35.0f; rHandZ = 0.0f;

		lLegX = ease * 50.0f; lLegY = 0.0f; lLegZ = 0.0f;
		lKneeX = ease * 20.0f;
		lFootX = 0.0f; lFootZ = 0.0f;

		rLegX = ease * 50.0f; rLegY = 0.0f; rLegZ = 0.0f;
		rKneeX = ease * 5.0f;
		rFootX = ease * 50.0f; rFootZ = 0.0f;

		charX = characterX; charY = baseY + ease * 0.3f; charZ = characterZ;
	}

	// ---------------------------------------------------------
	// PHASE 2: Action / Firing (40% to 60%)
	// Body stays static, Arms transition to Phase 2 numbers!
	// ---------------------------------------------------------
	else if (t <= 0.6f) {
		float phaseT = (t - 0.4f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// Head, Torso, and Legs remain locked in Phase 1 pose
		headX = -10.0f; headY = 0.0f; headZ = 0.0f;
		uTorsoX = 20.0f; uTorsoY = 0.0f; uTorsoZ = 0.0f;
		lTorsoX = 15.0f; lTorsoY = 0.0f; lTorsoZ = 0.0f;
		lLegX = 50.0f; lLegY = 0.0f; lLegZ = 0.0f;
		lKneeX = 20.0f; lFootX = 0.0f; lFootZ = 0.0f;
		rLegX = 50.0f; rLegY = 0.0f; rLegZ = 0.0f;
		rKneeX = 5.0f; rFootX = 50.0f; rFootZ = 0.0f;

		// Left Arm Dynamic Strike
		lArmX = 60.0f + (ease * (100.0f - 60.0f));
		lArmY = 60.0f; // Stays at 60
		lArmZ = 0.0f;
		lLowerArmX = -55.0f + (ease * (-50.0f - (-55.0f)));
		lLowerArmY = 115.0f + (ease * (0.0f - 115.0f));
		lHandX = -30.0f + (ease * (-15.0f - (-30.0f)));
		lHandY = 5.0f + (ease * (-5.0f - 5.0f));
		lHandZ = 0.0f;

		// Right Arm Dynamic Sweep
		rArmX = 5.0f + (ease * (0.0f - 5.0f));
		rArmY = -30.0f + (ease * (70.0f - (-30.0f)));
		rArmZ = 0.0f;
		rLowerArmX = 35.0f + (ease * (45.0f - 35.0f));
		rLowerArmY = 0.0f + (ease * (10.0f - 0.0f));
		rHandX = 20.0f + (ease * (0.0f - 20.0f));
		rHandY = -35.0f + (ease * (-60.0f - (-35.0f)));
		rHandZ = 0.0f;
		charY = baseY + 0.3f + ease * 0.1f;
	}

	// ---------------------------------------------------------
	// PHASE 3: Hold Pose (60% to 80%)
	// Locks the entire body into the final Phase 2 numbers!
	// ---------------------------------------------------------
	else if (t <= 0.8f) {
		headX = -10.0f; headY = 0.0f; headZ = 0.0f;
		uTorsoX = 20.0f; uTorsoY = 0.0f; uTorsoZ = 0.0f;
		lTorsoX = 15.0f; lTorsoY = 0.0f; lTorsoZ = 0.0f;

		lArmX = 100.0f; lArmY = 60.0f; lArmZ = 0.0f;
		lLowerArmX = -50.0f; lLowerArmY = 0.0f;
		lHandX = -15.0f; lHandY = -5.0f; lHandZ = 0.0f;

		rArmX = 0.0f; rArmY = 70.0f; rArmZ = 0.0f;
		rLowerArmX = 45.0f; rLowerArmY = 10.0f;
		rHandX = 0.0f; rHandY = -60.0f; rHandZ = 0.0f;

		lLegX = 50.0f; lLegY = 0.0f; lLegZ = 0.0f;
		lKneeX = 20.0f; lFootX = 0.0f; lFootZ = 0.0f;

		rLegX = 50.0f; rLegY = 0.0f; rLegZ = 0.0f;
		rKneeX = 5.0f; rFootX = 50.0f; rFootZ = 0.0f;
		charY = baseY + 0.4f;
	}

	// ---------------------------------------------------------
	// PHASE 4: Recovery (80% to 100%)
	// Eases all Phase 2 numbers back to 0.0f for looping!
	// ---------------------------------------------------------
	else {
		float phaseT = (t - 0.8f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = -10.0f - (ease * -10.0f);
		uTorsoX = 20.0f - (ease * 20.0f);
		lTorsoX = 15.0f - (ease * 15.0f);

		lArmX = 100.0f - (ease * 100.0f); lArmY = 60.0f - (ease * 60.0f);
		lLowerArmX = -50.0f - (ease * -50.0f); lLowerArmY = 0.0f;
		lHandX = -15.0f - (ease * -15.0f); lHandY = -5.0f - (ease * -5.0f);

		rArmX = 0.0f; rArmY = 70.0f - (ease * 70.0f);
		rLowerArmX = 45.0f - (ease * 45.0f); rLowerArmY = 10.0f - (ease * 10.0f);
		rHandX = 0.0f; rHandY = -60.0f - (ease * -60.0f);

		lLegX = 50.0f - (ease * 50.0f); lKneeX = 20.0f - (ease * 20.0f);

		rLegX = 50.0f - (ease * 50.0f); rKneeX = 5.0f - (ease * 5.0f); rFootX = 50.0f - (ease * 50.0f);
		charY = baseY + (0.4f * (1.0f - ease));
	}

	// ==========================================
	// 3. APPLY VARIABLES TO PARTS
	// ==========================================
	parts[HEAD].angleX = headX; parts[HEAD].angleY = headY; parts[HEAD].angleZ = headZ;
	parts[UPPER_TORSO].angleX = uTorsoX; parts[UPPER_TORSO].angleY = uTorsoY; parts[UPPER_TORSO].angleZ = uTorsoZ;
	parts[LOWER_TORSO].angleX = lTorsoX; parts[LOWER_TORSO].angleY = lTorsoY; parts[LOWER_TORSO].angleZ = lTorsoZ;
	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY; parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleX = lHandX; parts[LEFT_HAND].angleY = lHandY; parts[LEFT_HAND].angleZ = lHandZ;
	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY; parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY; parts[RIGHT_HAND].angleZ = rHandZ;
	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_UPPER_LEG].angleY = lLegY; parts[LEFT_UPPER_LEG].angleZ = lLegZ;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX; parts[LEFT_FOOT].angleZ = lFootZ;
	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_UPPER_LEG].angleY = rLegY; parts[RIGHT_UPPER_LEG].angleZ = rLegZ;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX; parts[RIGHT_FOOT].angleZ = rFootZ;
	characterX = charX; characterY = charY; characterZ = charZ;
}

void BalletAnimation() {
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 0;
	animFrame += (1.0f * animSpeed);
	float maxFrames = 120.0f; // Slightly longer for a graceful, slow dance!

	if (animFrame > maxFrames) {
		if (isLooping) animFrame = 0.0f;
		else {
			animFrame = maxFrames;
			isPlaying = false;
		}
	}

	float t = animFrame / maxFrames;

	// ==========================================
	// 1. DECLARE ALL VARIABLES (Set to 0)
	// ==========================================
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;
	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;
	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;
	float lLegX = 0.0f, lLegY = 0.0f, lLegZ = 0.0f;
	float lKneeX = 0.0f;
	float lFootX = 0.0f, lFootZ = 0.0f;
	float rLegX = 0.0f, rLegY = 0.0f, rLegZ = 0.0f;
	float rKneeX = 0.0f;
	float rFootX = 0.0f, rFootZ = 0.0f;
	float charX = 0.0f, charY = 0.0f, charZ = 0.0f;

	// ==========================================
	// 2. ANIMATION PHASES
	// ==========================================

	// ---------------------------------------------------------
	// PHASE 1: Preparation / Relevé (0% to 30%)
	// Rise up on tiptoes, softly lift arms
	// ---------------------------------------------------------
	if (t <= 0.3f) {
		float phaseT = t / 0.3f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// Head tilts up proudly
		headX = ease * -15.0f;

		// Arms lift softly to the front/sides
		lArmX = ease * -45.0f; lLowerArmX = ease * -10.0f;
		rArmX = ease * -45.0f; rLowerArmX = ease * -10.0f;

		// Up on toes! (Both feet point down)
		lFootX = ease * 30.0f;
		rFootX = ease * 30.0f;
		charY = ease * 0.05f; // Lift entire body slightly
	}

	// ---------------------------------------------------------
	// PHASE 2: Extend into Arabesque (30% to 60%)
	// Lean forward, right leg shoots back, arms spread wide
	// ---------------------------------------------------------
	else if (t <= 0.6f) {
		float phaseT = (t - 0.3f) / 0.3f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		// Start from Phase 1 peaks
		headX = -15.0f + (ease * (5.0f - (-15.0f))); // Look forward

		// Torso leans elegantly forward
		uTorsoX = ease * 35.0f;
		lTorsoX = ease * 15.0f;

		// Left arm reaches gracefully forward, right arm reaches slightly back
		lArmX = -45.0f + (ease * (-90.0f - (-45.0f)));
		rArmX = -45.0f + (ease * (20.0f - (-45.0f))); rArmY = ease * 40.0f;

		// Left leg stays straight on tiptoe, right leg lifts high in the back
		lFootX = 30.0f;
		rLegX = ease * -75.0f; // Lift leg way back!
		rKneeX = ease * 5.0f;  // Keep it mostly straight
		rFootX = 30.0f + (ease * (45.0f - 30.0f)); // Point toes even harder

		charY = 0.05f + (ease * (-0.02f - 0.05f)); // Dip slightly to balance
	}

	// ---------------------------------------------------------
	// PHASE 3: Hold the Pose (60% to 80%)
	// Hold the Arabesque
	// ---------------------------------------------------------
	else if (t <= 0.8f) {
		headX = 5.0f;
		uTorsoX = 35.0f; lTorsoX = 15.0f;
		lArmX = -90.0f; lLowerArmX = -10.0f;
		rArmX = 20.0f; rArmY = 40.0f; rLowerArmX = -10.0f;
		lFootX = 30.0f;
		rLegX = -75.0f; rKneeX = 5.0f; rFootX = 45.0f;
		charY = -0.02f;
	}

	// ---------------------------------------------------------
	// PHASE 4: Graceful Recovery (80% to 100%)
	// Lower leg, drop heels, return to standing
	// ---------------------------------------------------------
	else {
		float phaseT = (t - 0.8f) / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;

		headX = 5.0f - (ease * 5.0f);
		uTorsoX = 35.0f - (ease * 35.0f); lTorsoX = 15.0f - (ease * 15.0f);
		lArmX = -90.0f - (ease * -90.0f); lLowerArmX = -10.0f - (ease * -10.0f);
		rArmX = 20.0f - (ease * 20.0f); rArmY = 40.0f - (ease * 40.0f); rLowerArmX = -10.0f - (ease * -10.0f);
		lFootX = 30.0f - (ease * 30.0f);
		rLegX = -75.0f - (ease * -75.0f); rKneeX = 5.0f - (ease * 5.0f); rFootX = 45.0f - (ease * 45.0f);
		charY = -0.02f - (ease * -0.02f);
	}

	// ==========================================
	// 3. APPLY VARIABLES TO PARTS
	// ==========================================
	parts[HEAD].angleX = headX; parts[HEAD].angleY = headY; parts[HEAD].angleZ = headZ;
	parts[UPPER_TORSO].angleX = uTorsoX; parts[UPPER_TORSO].angleY = uTorsoY; parts[UPPER_TORSO].angleZ = uTorsoZ;
	parts[LOWER_TORSO].angleX = lTorsoX; parts[LOWER_TORSO].angleY = lTorsoY; parts[LOWER_TORSO].angleZ = lTorsoZ;
	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY; parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleX = lHandX; parts[LEFT_HAND].angleY = lHandY; parts[LEFT_HAND].angleZ = lHandZ;
	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY; parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY; parts[RIGHT_HAND].angleZ = rHandZ;
	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_UPPER_LEG].angleY = lLegY; parts[LEFT_UPPER_LEG].angleZ = lLegZ;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX; parts[LEFT_FOOT].angleZ = lFootZ;
	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_UPPER_LEG].angleY = rLegY; parts[RIGHT_UPPER_LEG].angleZ = rLegZ;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX; parts[RIGHT_FOOT].angleZ = rFootZ;
	//characterX = charX; characterY = charY; characterZ = charZ;
}

void GunShootAnimation() {
	// 1. SAFETY & INITIALIZATION
	if (!isPlaying || currentSceneMode != ANIMATION) {
		isGunShooting = false;
		if (currentRightWeapon == 4) currentRightWeapon = 0;
		return;
	}

	currentLeftWeapon = 0;
	currentRightWeapon = 4; // Force Magnum to be equipped

	float maxFrames = 90.0f;
	animFrame += (1.0f * animSpeed);

	// 2. LOOP LOGIC: Jump back to Phase 2 (Aiming) instead of 0
	if (animFrame > maxFrames) {
		if (isLooping) {
			animFrame = 0.2f * maxFrames; // Restarts at the "Aiming" stance
		}
		else {
			animFrame = maxFrames;
			isPlaying = false;
			isGunShooting = false;
			currentRightWeapon = 0;
			return;
		}
	}

	float t = animFrame / maxFrames;

	// 3. SHOOTING FLASH LOGIC
	if (t >= 0.20f && t <= 0.30f) {
		isGunShooting = true;
		shootTimer = 2; // Prevents WM_TIMER from killing the flash instantly
	}
	else {
		if (shootTimer <= 0) isGunShooting = false;
	}

	// 4. BONE TRANSFORM VARIABLES
	float headX = 0.0f, headY = 0.0f, headZ = 0.0f;
	float uTorsoX = 0.0f, uTorsoY = 0.0f, uTorsoZ = 0.0f;
	float lTorsoX = 0.0f, lTorsoY = 0.0f, lTorsoZ = 0.0f;
	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f;
	float lLowerArmX = 0.0f, lLowerArmY = 0.0f;
	float lHandY = 0.0f;
	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f;
	float rLowerArmX = 0.0f, rLowerArmY = 0.0f;
	float rHandX = 0.0f, rHandY = 0.0f;
	float lLegX = 0.0f, lKneeX = 0.0f, lFootX = 0.0f;
	float rLegX = 0.0f, rKneeX = 0.0f, rFootX = 0.0f;

	// 5. ANIMATION PHASES
	// Ease-in formula: $ease = \frac{1 - \cos(phaseT \cdot \pi)}{2}$

	if (t <= 0.2f) {
		// PHASE 1: Draw & Aim (Only plays on first run)
		float phaseT = t / 0.2f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;
		uTorsoX = ease * -5.0f;
		lArmX = ease * 55.0f; lArmY = ease * 80.0f;
		lLowerArmX = ease * -50.0f; lLowerArmY = ease * 75.0f;
		lHandY = ease * 25.0f;
		rArmY = ease * 90.0f;
		rLowerArmX = ease * 70.0f; rLowerArmY = ease * 5.0f;
		rHandX = ease * 20.0f;
		lLegX = ease * -25.0f; lKneeX = ease * 20.0f; lFootX = ease * 5.0f;
		rLegX = ease * 20.0f; rKneeX = ease * 15.0f; rFootX = ease * -5.0f;
	}
	else if (t <= 0.3f) {
		// PHASE 2: Shooting Stance (Hold Aim)
		uTorsoX = -5.0f;
		lArmX = 55.0f; lArmY = 80.0f;
		lLowerArmX = -50.0f; lLowerArmY = 75.0f;
		lHandY = 25.0f;
		rArmY = 90.0f;
		rLowerArmX = 70.0f; rLowerArmY = 5.0f;
		rHandX = 20.0f;
		lLegX = -25.0f; lKneeX = 20.0f; lFootX = 5.0f;
		rLegX = 20.0f; rKneeX = 15.0f; rFootX = -5.0f;
	}
	else if (t <= 0.4f) {
		// PHASE 3: Recoil Snap
		float phaseT = (t - 0.3f) / 0.1f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;
		uTorsoX = -5.0f + (ease * 5.0f);
		lArmX = 55.0f; lArmY = 80.0f;
		lLowerArmX = -50.0f; lLowerArmY = 75.0f;
		lHandY = 25.0f;
		rArmX = ease * 10.0f;
		rArmY = 90.0f;
		rLowerArmX = 70.0f;
		rLowerArmY = 5.0f + (ease * 10.0f);
		rHandX = 20.0f;
		rHandY = ease * 10.0f;
		lLegX = -25.0f; lKneeX = 20.0f; lFootX = 5.0f;
		rLegX = 20.0f; rKneeX = 15.0f; rFootX = -5.0f;
	}
	else if (t <= 0.6f) {
		// PHASE 4: Hold Recoil
		uTorsoX = 0.0f;
		lArmX = 55.0f; lArmY = 80.0f;
		lLowerArmX = -50.0f; lLowerArmY = 75.0f;
		lHandY = 25.0f;
		rArmX = 10.0f; rArmY = 90.0f;
		rLowerArmX = 70.0f; rLowerArmY = 15.0f;
		rHandX = 20.0f; rHandY = 10.0f;
		lLegX = -25.0f; lKneeX = 20.0f; lFootX = 5.0f;
		rLegX = 20.0f; rKneeX = 15.0f; rFootX = -5.0f;
	}
	else {
		// PHASE 5: Recovery (Return to AIMING pose)
		float phaseT = (t - 0.6f) / 0.4f;
		float ease = (1.0f - cos(phaseT * 3.14159f)) / 2.0f;
		uTorsoX = 0.0f + (ease * -5.0f);
		lArmX = 55.0f; lArmY = 80.0f;
		lLowerArmX = -50.0f; lLowerArmY = 75.0f;
		lHandY = 25.0f;
		rArmX = 10.0f + (ease * -10.0f);
		rArmY = 90.0f;
		rLowerArmX = 70.0f;
		rLowerArmY = 15.0f + (ease * -10.0f);
		rHandX = 20.0f;
		rHandY = 10.0f + (ease * -10.0f);
		lLegX = -25.0f; lKneeX = 20.0f; lFootX = 5.0f;
		rLegX = 20.0f; rKneeX = 15.0f; rFootX = -5.0f;
	}

	// 6. APPLY TO CHARACTER
	parts[UPPER_TORSO].angleX = uTorsoX;
	parts[LEFT_UPPER_ARM].angleX = lArmX; parts[LEFT_UPPER_ARM].angleY = lArmY;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX; parts[LEFT_LOWER_ARM].angleY = lLowerArmY;
	parts[LEFT_HAND].angleY = lHandY;
	parts[RIGHT_UPPER_ARM].angleX = rArmX; parts[RIGHT_UPPER_ARM].angleY = rArmY;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX; parts[RIGHT_LOWER_ARM].angleY = rLowerArmY;
	parts[RIGHT_HAND].angleX = rHandX; parts[RIGHT_HAND].angleY = rHandY;
	parts[LEFT_UPPER_LEG].angleX = lLegX; parts[LEFT_LOWER_LEG].angleX = lKneeX; parts[LEFT_FOOT].angleX = lFootX;
	parts[RIGHT_UPPER_LEG].angleX = rLegX; parts[RIGHT_LOWER_LEG].angleX = rKneeX; parts[RIGHT_FOOT].angleX = rFootX;
}

void BackFlipAnimation()
{
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 0;

	float maxFrames = 90.0f;
	animFrame += (1.0f * animSpeed);

	// Handle Looping logic
	if (animFrame > maxFrames)
	{
		if (isLooping)
		{
			animFrame = 0.0f; // Restart
		}
		else
		{
			animFrame = maxFrames; // Lock it at the last frame
			isPlaying = false;	   // Auto-pause at the end
		}
	}

	float t = animFrame / maxFrames;

	float torsoX = 0.0f, torsoY = 0.0f, lowerTorsoX = 0.0f, headX = 0.0f;
	float rArmX = 0.0f, rArmY = 0.0f, rArmZ = 0.0f, rLowerArmX = 0.0f, rHandX = 0.0f, rHandY = 0.0f, rHandZ = 0.0f;
	float lArmX = 0.0f, lArmY = 0.0f, lArmZ = 0.0f, lLowerArmX = 0.0f, lHandX = 0.0f, lHandY = 0.0f, lHandZ = 0.0f;
	float lLegX = 0.0f, rLegX = 0.0f, lKneeX = 0.0f, rKneeX = 0.0f, lFootX = 0.0f, rFootX = 0.0f;
	float charZ = 0.0f, charY = 0.0f;

	if (t <= 0.18f)
	{
		float phaseT = t / 0.18f;
		float bodyEase = (1.0f - cosf(phaseT * PI)) * 0.5f;

		torsoX = 22.0f * bodyEase;
		lowerTorsoX = 14.0f * bodyEase;
		headX = -8.0f * bodyEase;

		if (phaseT <= 0.45f)
		{
			float armT = phaseT / 0.45f;
			float armEase = (1.0f - cosf(armT * PI)) * 0.5f;

			lArmX = 18.0f * armEase;
			rArmX = 18.0f * armEase;
			lArmY = -6.0f * armEase;
			rArmY = 6.0f * armEase;
			lLowerArmX = -10.0f * armEase;
			rLowerArmX = -10.0f * armEase;
		}
		else
		{
			float armT = (phaseT - 0.45f) / 0.55f;
			float armEase = (1.0f - cosf(armT * PI)) * 0.5f;

			lArmX = 18.0f + (-80.0f * armEase);
			rArmX = 18.0f + (-80.0f * armEase);
			lArmY = -6.0f + (-10.0f * armEase);
			rArmY = 6.0f + (10.0f * armEase);
			lLowerArmX = -10.0f + (-18.0f * armEase);
			rLowerArmX = -10.0f + (-18.0f * armEase);
		}

		lLegX = -22.0f * bodyEase;
		rLegX = -22.0f * bodyEase;
		lKneeX = 26.0f * bodyEase;
		rKneeX = 26.0f * bodyEase;
		lFootX = -10.0f * bodyEase;
		rFootX = -10.0f * bodyEase;

		charY = -0.13f - 0.05f * bodyEase;
		charZ = -1.0f;
	}
	else if (t <= 0.82f)
	{
		float airT = (t - 0.18f) / 0.64f;
		float rotation = 360.0f * airT;
		float tuck = sinf(airT * PI);
		float twist = sinf(airT * PI * 3.0f);

		torsoX = 22.0f + rotation;
		lowerTorsoX = 14.0f + rotation * 0.90f;
		headX = -8.0f + rotation * 0.18f - tuck * 12.0f;

		lArmX = -62.0f;
		rArmX = -62.0f;
		lArmY = -16.0f;
		rArmY = 16.0f;
		lLowerArmX = -28.0f;
		rLowerArmX = -28.0f;

		lLegX = -22.0f - tuck * 58.0f;
		rLegX = -22.0f - tuck * 58.0f;
		lKneeX = 26.0f + tuck * 82.0f + twist * 8.0f;
		rKneeX = 26.0f + tuck * 82.0f - twist * 8.0f;
		lFootX = -10.0f + tuck * 24.0f;
		rFootX = -10.0f + tuck * 24.0f;

		charY = -0.18f + sinf(airT * PI) * 0.55f;
		charZ = -1.0f + sinf(airT * PI) * 0.04f;
	}
	else
	{
		torsoX = 360.0f;
		lowerTorsoX = 360.0f;
		charY = -0.13f;
		charZ = -1.0f;
	}

	// Apply the calculated angles to the actual character parts
	parts[HEAD].angleX = headX;
	parts[UPPER_TORSO].angleX = torsoX;
	parts[UPPER_TORSO].angleY = torsoY;
	parts[LOWER_TORSO].angleX = lowerTorsoX;

	parts[RIGHT_UPPER_ARM].angleX = rArmX;
	parts[RIGHT_UPPER_ARM].angleY = rArmY;
	parts[RIGHT_UPPER_ARM].angleZ = rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = rLowerArmX;
	parts[RIGHT_HAND].angleX = rHandX;
	parts[RIGHT_HAND].angleY = rHandY;
	parts[RIGHT_HAND].angleZ = rHandZ;

	parts[LEFT_UPPER_ARM].angleX = lArmX;
	parts[LEFT_UPPER_ARM].angleY = lArmY;
	parts[LEFT_UPPER_ARM].angleZ = lArmZ;
	parts[LEFT_LOWER_ARM].angleX = lLowerArmX;
	parts[LEFT_HAND].angleX = lHandX;
	parts[LEFT_HAND].angleY = lHandY;
	parts[LEFT_HAND].angleZ = lHandZ;

	parts[LEFT_UPPER_LEG].angleX = lLegX;
	parts[LEFT_LOWER_LEG].angleX = lKneeX;
	parts[LEFT_FOOT].angleX = lFootX;
	parts[RIGHT_UPPER_LEG].angleX = rLegX;
	parts[RIGHT_LOWER_LEG].angleX = rKneeX;
	parts[RIGHT_FOOT].angleX = rFootX;

	characterZ = charZ;
	characterY = charY;
}

void SixSevenAnimation()
{
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 0;

	float maxFrames = 90.0f;
	animFrame += (1.0f * animSpeed);

	// Handle Looping logic
	if (animFrame > maxFrames)
	{
		if (isLooping)
		{
			animFrame = 0.0f; // Restart
		}
		else
		{
			animFrame = maxFrames; // Lock it at the last frame
			isPlaying = false;	   // Auto-pause at the end
		}
	}

	float t = animFrame / maxFrames;

	AnimationPose pose;

	float wave = sinf(t * PI * 2.0f);
	float bodyBounce = sinf(t * PI * 4.0f) * 0.01f;

	pose.torsoY = 0.0f;
	pose.headX = 0.0f;

	// upper arms
	pose.lArmX = -14.0f + wave * 18.0f;
	pose.rArmX = -14.0f + wave * 18.0f;

	pose.lArmY = 88.0f;
	pose.rArmY = 88.0f;

	pose.lArmZ = 18.0f + wave * 12.0f;
	pose.rArmZ = 18.0f + wave * 12.0f;

	// lower arms
	pose.lLowerArmX = 54.0f + wave * 12.0f;
	pose.rLowerArmX = 54.0f + wave * 12.0f;

	// hands
	pose.lHandX = wave * 15.0f;
	pose.rHandX = wave * 15.0f;

	pose.lHandY = wave * 8.0f;
	pose.rHandY = wave * 8.0f;

	pose.lHandZ = wave * 10.0f;
	pose.rHandZ = wave * 10.0f;

	// legs
	pose.lLegX = 0.0f;
	pose.rLegX = 0.0f;
	pose.lKneeX = 10.0f;
	pose.rKneeX = 10.0f;
	pose.lFootX = 0.0f;
	pose.rFootX = 0.0f;

	pose.charY = -0.13f + bodyBounce;
	pose.charZ = -1.0f;

	// Apply the calculated angles to the actual character parts
	parts[HEAD].angleX = pose.headX;
	parts[UPPER_TORSO].angleX = pose.torsoX;
	parts[UPPER_TORSO].angleY = pose.torsoY;
	parts[LOWER_TORSO].angleX = pose.lowerTorsoX;

	parts[RIGHT_UPPER_ARM].angleX = pose.rArmX;
	parts[RIGHT_UPPER_ARM].angleY = pose.rArmY;
	parts[RIGHT_UPPER_ARM].angleZ = pose.rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = pose.rLowerArmX;
	parts[RIGHT_HAND].angleX = pose.rHandX;
	parts[RIGHT_HAND].angleY = pose.rHandY;
	parts[RIGHT_HAND].angleZ = pose.rHandZ;

	parts[LEFT_UPPER_ARM].angleX = pose.lArmX;
	parts[LEFT_UPPER_ARM].angleY = pose.lArmY;
	parts[LEFT_UPPER_ARM].angleZ = pose.lArmZ;
	parts[LEFT_LOWER_ARM].angleX = pose.lLowerArmX;
	parts[LEFT_HAND].angleX = pose.lHandX;
	parts[LEFT_HAND].angleY = pose.lHandY;
	parts[LEFT_HAND].angleZ = pose.lHandZ;

	parts[LEFT_UPPER_LEG].angleX = pose.lLegX;
	parts[LEFT_LOWER_LEG].angleX = pose.lKneeX;
	parts[LEFT_FOOT].angleX = pose.lFootX;
	parts[RIGHT_UPPER_LEG].angleX = pose.rLegX;
	parts[RIGHT_LOWER_LEG].angleX = pose.rKneeX;
	parts[RIGHT_FOOT].angleX = pose.rFootX;

	characterZ = pose.charZ;
	characterY = pose.charY;
}

void EnergyBeamAnimation()
{
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 0;
	currentRightWeapon = 0;

	float maxFrames = 90.0f;
	animFrame += (1.0f * animSpeed);

	// Handle Looping logic
	if (animFrame > maxFrames)
	{
		if (isLooping)
		{
			animFrame = 0.0f; // Restart
		}
		else
		{
			animFrame = maxFrames; // Lock it at the last frame
			isPlaying = false;	   // Auto-pause at the end
		}
	}

	float t = animFrame / maxFrames;

	AnimationPose pose;

	// Phase 1: Charge pose
	if (t <= 0.35f)
	{
		float phaseT = t / 0.35f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		pose.torsoY = -18.0f * ease;
		pose.torsoX = 6.0f * ease;
		pose.lowerTorsoX = 4.0f * ease;
		pose.headX = -6.0f * ease;

		// Both hands pull to the side of body
		pose.lArmX = -25.0f * ease;
		pose.lArmY = 65.0f * ease;
		pose.lArmZ = -35.0f * ease;

		pose.rArmX = -25.0f * ease;
		pose.rArmY = 65.0f * ease;
		pose.rArmZ = 35.0f * ease;

		pose.lLowerArmX = 70.0f * ease;
		pose.rLowerArmX = 70.0f * ease;

		pose.lHandX = 20.0f * ease;
		pose.rHandX = 20.0f * ease;

		pose.lLegX = 18.0f * ease;
		pose.rLegX = -10.0f * ease;
		pose.lKneeX = 20.0f * ease;
		pose.rKneeX = 10.0f * ease;
		pose.lFootX = -8.0f * ease;
		pose.rFootX = 4.0f * ease;

		pose.charY = -0.13f - 0.03f * ease;
		pose.charZ = -1.0f;
	}
	// Phase 2: Hold and shake while charging
	else if (t <= 0.60f)
	{
		float phaseT = (t - 0.35f) / 0.25f;
		float shake = sinf(phaseT * PI * 10.0f) * 3.0f;
		float pulse = sinf(phaseT * PI * 6.0f) * 2.0f;

		pose.torsoY = -18.0f + shake * 0.3f;
		pose.torsoX = 6.0f + shake * 0.2f;
		pose.lowerTorsoX = 4.0f;
		pose.headX = -6.0f - pulse * 0.3f;

		pose.lArmX = -25.0f + pulse;
		pose.lArmY = 65.0f;
		pose.lArmZ = -35.0f + shake;

		pose.rArmX = -25.0f + pulse;
		pose.rArmY = 65.0f;
		pose.rArmZ = 35.0f - shake;

		pose.lLowerArmX = 70.0f + pulse;
		pose.rLowerArmX = 70.0f + pulse;

		pose.lHandX = 20.0f + pulse;
		pose.rHandX = 20.0f + pulse;

		pose.lLegX = 18.0f;
		pose.rLegX = -10.0f;
		pose.lKneeX = 20.0f;
		pose.rKneeX = 10.0f;
		pose.lFootX = -8.0f;
		pose.rFootX = 4.0f;

		pose.charY = -0.16f + sinf(phaseT * PI * 8.0f) * 0.005f;
		pose.charZ = -1.0f;
	}
	// Phase 3: Fire beam forward
	else if (t <= 0.82f)
	{
		float phaseT = (t - 0.60f) / 0.22f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		pose.torsoY = -18.0f + 28.0f * ease;
		pose.torsoX = 6.0f - 10.0f * ease;
		pose.lowerTorsoX = 4.0f - 6.0f * ease;
		pose.headX = -6.0f + 10.0f * ease;

		// Push both hands forward
		pose.lArmX = -25.0f + (-70.0f * ease);
		pose.lArmY = 65.0f - 65.0f * ease;
		pose.lArmZ = -35.0f + 35.0f * ease;

		pose.rArmX = -25.0f + (-70.0f * ease);
		pose.rArmY = 65.0f - 65.0f * ease;
		pose.rArmZ = 35.0f - 35.0f * ease;

		pose.lLowerArmX = 70.0f - 40.0f * ease;
		pose.rLowerArmX = 70.0f - 40.0f * ease;

		pose.lHandX = 20.0f - 20.0f * ease;
		pose.rHandX = 20.0f - 20.0f * ease;

		pose.lLegX = 18.0f - 28.0f * ease;
		pose.rLegX = -10.0f + 25.0f * ease;
		pose.lKneeX = 20.0f + 18.0f * ease;
		pose.rKneeX = 10.0f - 10.0f * ease;
		pose.lFootX = -8.0f - 4.0f * ease;
		pose.rFootX = 4.0f;

		pose.charY = -0.16f - 0.02f * ease;
		pose.charZ = -1.0f + 0.18f * ease;
	}
	// Phase 4: End pose / recovery
	else
	{
		float phaseT = (t - 0.82f) / 0.18f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		// Start from firing pose back to neutral
		pose.torsoY = 10.0f - 10.0f * ease;
		pose.torsoX = -4.0f + 4.0f * ease;
		pose.lowerTorsoX = -2.0f + 2.0f * ease;
		pose.headX = 4.0f - 4.0f * ease;

		pose.lArmX = -95.0f + 95.0f * ease;
		pose.rArmX = -95.0f + 95.0f * ease;

		pose.lArmY = 0.0f;
		pose.rArmY = 0.0f;
		pose.lArmZ = 0.0f;
		pose.rArmZ = 0.0f;

		pose.lLowerArmX = 30.0f - 30.0f * ease;
		pose.rLowerArmX = 30.0f - 30.0f * ease;

		pose.lHandX = 0.0f;
		pose.rHandX = 0.0f;

		pose.lLegX = -10.0f + 10.0f * ease;
		pose.rLegX = 15.0f - 15.0f * ease;
		pose.lKneeX = 38.0f - 38.0f * ease;
		pose.rKneeX = 0.0f;
		pose.lFootX = -12.0f + 12.0f * ease;
		pose.rFootX = 0.0f;

		pose.charY = -0.18f + 0.05f * ease;
		pose.charZ = -0.82f - 0.18f * ease;
	}

	// Apply the calculated angles to the actual character parts
	parts[HEAD].angleX = pose.headX;
	parts[UPPER_TORSO].angleX = pose.torsoX;
	parts[UPPER_TORSO].angleY = pose.torsoY;
	parts[LOWER_TORSO].angleX = pose.lowerTorsoX;

	parts[RIGHT_UPPER_ARM].angleX = pose.rArmX;
	parts[RIGHT_UPPER_ARM].angleY = pose.rArmY;
	parts[RIGHT_UPPER_ARM].angleZ = pose.rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = pose.rLowerArmX;
	parts[RIGHT_HAND].angleX = pose.rHandX;
	parts[RIGHT_HAND].angleY = pose.rHandY;
	parts[RIGHT_HAND].angleZ = pose.rHandZ;

	parts[LEFT_UPPER_ARM].angleX = pose.lArmX;
	parts[LEFT_UPPER_ARM].angleY = pose.lArmY;
	parts[LEFT_UPPER_ARM].angleZ = pose.lArmZ;
	parts[LEFT_LOWER_ARM].angleX = pose.lLowerArmX;
	parts[LEFT_HAND].angleX = pose.lHandX;
	parts[LEFT_HAND].angleY = pose.lHandY;
	parts[LEFT_HAND].angleZ = pose.lHandZ;

	parts[LEFT_UPPER_LEG].angleX = pose.lLegX;
	parts[LEFT_LOWER_LEG].angleX = pose.lKneeX;
	parts[LEFT_FOOT].angleX = pose.lFootX;
	parts[RIGHT_UPPER_LEG].angleX = pose.rLegX;
	parts[RIGHT_LOWER_LEG].angleX = pose.rKneeX;
	parts[RIGHT_FOOT].angleX = pose.rFootX;

	characterZ = pose.charZ;
	characterY = pose.charY;
}

void DualSwordRushAnimation()
{
	if (!isPlaying || currentSceneMode != ANIMATION) return;

	currentLeftWeapon = 5;
	currentRightWeapon = 5;
	float maxFrames = 96.0f;
	animFrame += (1.0f * animSpeed);

	if (animFrame > maxFrames)
	{
		if (isLooping)
			animFrame = 0.0f;
		else
		{
			animFrame = maxFrames;
			isPlaying = false;
		}
	}

	float t = animFrame / maxFrames;
	AnimationPose pose;

	if (t <= 0.22f)
	{
		float phaseT = t / 0.22f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		pose.torsoY = -18.0f * ease;
		pose.torsoX = 8.0f * ease;
		pose.lowerTorsoX = 5.0f * ease;
		pose.lowerTorsoY = pose.torsoY;
		pose.headX = -4.0f * ease;

		pose.rArmX = 55.0f * ease;
		pose.rArmY = 18.0f * ease;
		pose.rArmZ = 45.0f * ease;
		pose.rLowerArmX = 18.0f * ease;
		pose.rHandX = 25.0f * ease;
		pose.rHandY = -25.0f * ease;

		pose.lArmX = -20.0f * ease;
		pose.lArmY = 75.0f * ease;
		pose.lArmZ = -28.0f * ease;
		pose.lLowerArmX = 35.0f * ease;
		pose.lHandX = -15.0f * ease;
		pose.lHandY = 12.0f * ease;

		pose.lLegX = 22.0f * ease;
		pose.rLegX = -14.0f * ease;
		pose.lKneeX = 20.0f * ease;
		pose.rKneeX = 10.0f * ease;
		pose.lFootX = -8.0f * ease;
		pose.rFootX = 5.0f * ease;

		pose.charY = -0.13f - 0.03f * ease;
		pose.charZ = -1.0f;
	}
	else if (t <= 0.46f)
	{
		float phaseT = (t - 0.22f) / 0.24f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		pose.torsoY = -18.0f + (-48.0f * ease);
		pose.torsoX = 8.0f + 10.0f * ease;
		pose.lowerTorsoX = 5.0f + 6.0f * ease;
		pose.lowerTorsoY = pose.torsoY;
		pose.headX = -4.0f + 10.0f * ease;

		pose.rArmX = 55.0f + (-120.0f * ease);
		pose.rArmY = 18.0f + 55.0f * ease;
		pose.rArmZ = 45.0f + (-70.0f * ease);
		pose.rLowerArmX = 18.0f + 12.0f * ease;
		pose.rHandX = 25.0f + 20.0f * ease;
		pose.rHandY = -25.0f + 35.0f * ease;

		pose.lArmX = -20.0f + 40.0f * ease;
		pose.lArmY = 75.0f + (-20.0f * ease);
		pose.lArmZ = -28.0f + 35.0f * ease;
		pose.lLowerArmX = 35.0f + 5.0f * ease;
		pose.lHandX = -15.0f + 10.0f * ease;
		pose.lHandY = 12.0f + (-20.0f * ease);

		pose.lLegX = 22.0f + (-32.0f * ease);
		pose.rLegX = -14.0f + 34.0f * ease;
		pose.lKneeX = 20.0f + 16.0f * ease;
		pose.rKneeX = 10.0f;
		pose.lFootX = -8.0f + (-5.0f * ease);
		pose.rFootX = 5.0f;

		pose.charY = -0.16f - 0.02f * ease;
		pose.charZ = -1.0f + 0.12f * ease;
	}
	else if (t <= 0.72f)
	{
		float phaseT = (t - 0.46f) / 0.26f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;
		float shake = sinf(phaseT * PI * 8.0f) * 3.0f;

		pose.torsoY = -66.0f + 92.0f * ease;
		pose.torsoX = 18.0f - 14.0f * ease;
		pose.lowerTorsoX = 11.0f - 8.0f * ease;
		pose.lowerTorsoY = pose.torsoY;
		pose.headX = 6.0f - 8.0f * ease;

		pose.rArmX = -65.0f + 35.0f * ease;
		pose.rArmY = 73.0f - 20.0f * ease;
		pose.rArmZ = -25.0f + 55.0f * ease;
		pose.rLowerArmX = 30.0f - 15.0f * ease;
		pose.rHandX = 45.0f - 22.0f * ease;
		pose.rHandY = 10.0f - 15.0f * ease;

		pose.lArmX = 20.0f + (-115.0f * ease);
		pose.lArmY = 55.0f + (-42.0f * ease);
		pose.lArmZ = 7.0f + 68.0f * ease;
		pose.lLowerArmX = 40.0f - 10.0f * ease;
		pose.lHandX = -5.0f - 25.0f * ease;
		pose.lHandY = -8.0f - 25.0f * ease;

		pose.lLegX = -10.0f + 28.0f * ease;
		pose.rLegX = 20.0f - 35.0f * ease;
		pose.lKneeX = 36.0f - 10.0f * ease;
		pose.rKneeX = 10.0f + 18.0f * ease;
		pose.lFootX = -13.0f + 8.0f * ease;
		pose.rFootX = 5.0f - 10.0f * ease;

		pose.charY = -0.18f + sinf(phaseT * PI * 4.0f) * 0.01f;
		pose.charZ = -0.88f - 0.08f * ease + shake * 0.0008f;
	}
	else
	{
		float phaseT = (t - 0.72f) / 0.28f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;

		pose.torsoY = 26.0f - 26.0f * ease;
		pose.torsoX = 4.0f - 4.0f * ease;
		pose.lowerTorsoX = 3.0f - 3.0f * ease;
		pose.lowerTorsoY = pose.torsoY;
		pose.headX = -2.0f + 2.0f * ease;

		pose.rArmX = -30.0f + 30.0f * ease;
		pose.rArmY = 53.0f - 53.0f * ease;
		pose.rArmZ = 30.0f - 30.0f * ease;
		pose.rLowerArmX = 15.0f - 15.0f * ease;
		pose.rHandX = 23.0f - 23.0f * ease;
		pose.rHandY = -5.0f + 5.0f * ease;

		pose.lArmX = -95.0f + 95.0f * ease;
		pose.lArmY = 13.0f - 13.0f * ease;
		pose.lArmZ = 75.0f - 75.0f * ease;
		pose.lLowerArmX = 30.0f - 30.0f * ease;
		pose.lHandX = -30.0f + 30.0f * ease;
		pose.lHandY = -33.0f + 33.0f * ease;

		pose.lLegX = 18.0f - 18.0f * ease;
		pose.rLegX = -15.0f + 15.0f * ease;
		pose.lKneeX = 26.0f - 26.0f * ease;
		pose.rKneeX = 28.0f - 28.0f * ease;
		pose.lFootX = -5.0f + 5.0f * ease;
		pose.rFootX = -5.0f + 5.0f * ease;

		pose.charY = -0.17f + 0.04f * ease;
		pose.charZ = -0.96f - 0.04f * ease;
	}

	parts[HEAD].angleX = pose.headX;
	parts[UPPER_TORSO].angleX = pose.torsoX;
	parts[UPPER_TORSO].angleY = pose.torsoY;
	parts[LOWER_TORSO].angleX = pose.lowerTorsoX;
	parts[LOWER_TORSO].angleY = pose.lowerTorsoY;

	parts[RIGHT_UPPER_ARM].angleX = pose.rArmX;
	parts[RIGHT_UPPER_ARM].angleY = pose.rArmY;
	parts[RIGHT_UPPER_ARM].angleZ = pose.rArmZ;
	parts[RIGHT_LOWER_ARM].angleX = pose.rLowerArmX;
	parts[RIGHT_HAND].angleX = pose.rHandX;
	parts[RIGHT_HAND].angleY = pose.rHandY;
	parts[RIGHT_HAND].angleZ = pose.rHandZ;

	parts[LEFT_UPPER_ARM].angleX = pose.lArmX;
	parts[LEFT_UPPER_ARM].angleY = pose.lArmY;
	parts[LEFT_UPPER_ARM].angleZ = pose.lArmZ;
	parts[LEFT_LOWER_ARM].angleX = pose.lLowerArmX;
	parts[LEFT_HAND].angleX = pose.lHandX;
	parts[LEFT_HAND].angleY = pose.lHandY;
	parts[LEFT_HAND].angleZ = pose.lHandZ;

	parts[LEFT_UPPER_LEG].angleX = pose.lLegX;
	parts[LEFT_LOWER_LEG].angleX = pose.lKneeX;
	parts[LEFT_FOOT].angleX = pose.lFootX;
	parts[RIGHT_UPPER_LEG].angleX = pose.rLegX;
	parts[RIGHT_LOWER_LEG].angleX = pose.rKneeX;
	parts[RIGHT_FOOT].angleX = pose.rFootX;

	characterZ = pose.charZ;
	characterY = pose.charY;
}

void UpdateAnimation() {
	switch (currentAnimType)
	{
	case 0:
		WalkAnimation();
		break;
	case 1:
		SpearAttack();
		break;
	case 2:
		SlashAnimation();
		break;
	case 3:
		FirewheelAnimation();
		break;
	case 4:
		BalletAnimation();
		break;
	case 5:
		GunShootAnimation();
		break;
	case 6:
		BackFlipAnimation();
		break;
	case 7:
		SixSevenAnimation();
		break;
	case 8:
		EnergyBeamAnimation();
		break;
	case 9:
		DualSwordRushAnimation();
		break;
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

		// === NEW: GUN SHOOTING TIMER LOGIC ===
		if (isGunShooting) {
			shootTimer--; // Count down 1 frame
			if (shootTimer <= 0) {
				isGunShooting = false; // Turn off the fire effect when timer hits 0
			}
		}
		InvalidateRect(hWnd, NULL, FALSE); // redraw
		break;

	case WM_KEYDOWN:
	{
		PartRotation& part = parts[currentPart];

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

		case 0x31: // Press 1 - Interactive Movements
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = INTERACT;
			currentPart = HEAD;
			break;

		case 0x32: // Press 2 - ANIMATION MODE
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			ResetToggle();
			ResetModel();

			currentSceneMode = ANIMATION;
			currentAnimType = 0;
			isPlaying = true;
			animFrame = 0.0f;  // Reset to the beginning
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
		case 0x46: // 'F' Key - SHOOT MAGNUM
			if (currentSceneMode == WEAPON_CUSTOM) {
				isGunShooting = true;
				shootTimer = 8; // 33 frames * 30ms = ~1 second
			}
			break;
		case 0x4D: // 'M' Key
			if (currentSceneMode == WEAPON_CUSTOM)
			{
				wheelRotationSpeed -= 0.05f; // Decelerate / spin backward
			}
			break;
		case 'T': // 'T' Key - Degrip
			currentGrip += 2.0f; // Uncurl the fingers
			// Stop them from bending backwards
			if (currentGrip > 70.0f)
			{
				currentGrip = 70.0f;
			}
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

		case 0x4F: // [O]
			if (isCameraMode)
			{
				currentCameraMode = (CameraMode)((currentCameraMode + CAMERA_COUNT - 1) % CAMERA_COUNT);
			}
			else if (isLightMode)
			{
				lightIndex = (lightIndex + NUM_LIGHTS - 1) % NUM_LIGHTS;
			}
			else
			{
				switch (currentSceneMode)
				{
				case ANIMATION:
					isPlaying = true; // Play/Continue 
					break;
				}
			}
			break;
		case 0x50: // [P]
			if (isCameraMode)
			{
				currentCameraMode = (CameraMode)((currentCameraMode + 1) % CAMERA_COUNT);
			}
			else if (isLightMode)
			{
				lightIndex = (lightIndex + 1) % NUM_LIGHTS;
			}
			else
			{
				switch (currentSceneMode)
				{
				case ANIMATION:
					isPlaying = false; // Pause animation
					break;
				}
			}
			break;

		case 0x4C: // [L]
			if (currentSceneMode == ANIMATION)
			{
				isLooping = !isLooping; // Toggle looping on/off
			}
			else if (isLightMode)
			{
				ToggleLight(); // Normal Light Toggle
			}
			break;
		case 0x55: // [U] - SLOW DOWN
			if (currentSceneMode == ANIMATION)
			{
				animSpeed -= 0.25f;
				if (animSpeed < 0.25f)
					animSpeed = 0.25f; // Don't let it go backwards/stop entirely
			}
			break;

		case 0x49: // [I] - SPEED UP
			if (currentSceneMode == ANIMATION)
			{
				animSpeed += 0.25f;
			}
			break;

		case 0x5A: // [Z] Key
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
				ResetModel();
				// Switch to Previous Animation
				currentAnimType--;
				if (currentAnimType < 0) currentAnimType = 9; // Loop to the end (1 is Walk)

				animFrame = 0.0f; // Reset timeline so the new animation starts properly
				isPlaying = true; // Auto-play when switching
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

		case 0x58: // [X] Key
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
				ResetModel();
				// Switch to Next Animation
				currentAnimType++;
				if (currentAnimType > 9) currentAnimType = 0; // Loop back to start (0 is Spear)

				animFrame = 0.0f; // Reset timeline so the new animation starts properly
				isPlaying = true; // Auto-play when switching
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

		case 0x42: // [B] Key pressed
			switch (currentSceneMode)
			{
			case CHARACTER_CUSTOM:
				// (You can add logic here later if 'B' should do something else in Character mode)
				currentCharacterMode = (currentCharacterMode == LENGZAI_MODE) ? CHIBI_MODE : LENGZAI_MODE;
				break;
			case WEAPON_CUSTOM:
				// Cycle weapons ONLY when in Weapon Mode
				currentRightWeapon = (currentRightWeapon + 1) % TOTAL_WEAPONS;
				break;
			}
			break;
		case 0x47: // [G] Key pressed
			if (currentSceneMode == WEAPON_CUSTOM)
			{
				// Cycle left hand weapons (0 through 4)
				currentLeftWeapon = (currentLeftWeapon + 1) % 6;
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
		-60.0f, 60.0f,
		-8.0f, 8.0f
	};

	parts[LOWER_TORSO] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-20.0f, 5.0f,
		-60.0f, 60.0f,
		-8.0f, 8.0f
	};

	// LEFT ARM
	parts[LEFT_UPPER_ARM] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

	-60.0f, 100.0f,
		-30.0f, 100.0f,
		-80.0f, 80.0f
	};

	parts[LEFT_LOWER_ARM] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-90.0f, 10.0f,
		0.0f, 130.0f,
		0.0f, 0.0f
	};

	parts[LEFT_HAND] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-100.0f, 10.0f,
		-90.0f, 90.0f,
		-50.0f, 80.0f
	};

	// RIGHT ARM
	parts[RIGHT_UPPER_ARM] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-60.0f, 100.0f,
		-30.0f, 100.0f,
		-80.0f, 80.0f
	};

	parts[RIGHT_LOWER_ARM] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-10.0f, 70.0f,
		0.0f, 130.0f,
		0.0f, 0.0f
	};

	parts[RIGHT_HAND] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-10.0f, 100.0f,
		-90.0f, 90.0f,
		-80.0f, 50.0f
	};

	// LEFT LEG
	parts[LEFT_UPPER_LEG] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-50.0f, 50.0f,
		-10.0f, 20.0f,
		-10.0f, 50.0f
	};

	parts[LEFT_LOWER_LEG] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		0.0f, 120.0f,
		0.0f, 0.0f,
		0.0f, 0.0f
	};

	parts[LEFT_FOOT] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-5.0f, 50.0f,
		0.0f, 0.0f,
		-30.0f, 5.0f
	};

	// RIGHT LEG
	parts[RIGHT_UPPER_LEG] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-50.0f, 50.0f,
		-10.0f, 20.0f,
		-10.0f, 50.0f
	};

	parts[RIGHT_LOWER_LEG] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		0.0f, 120.0f,
		0.0f, 0.0f,
		0.0f, 0.0f };

	parts[RIGHT_FOOT] =
	{
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,

		-5.0f, 50.0f,
		0.0f, 0.0f,
		-30.0f, 5.0f
	};
}

GLuint LoadTexture(const char* filePath)
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
	chainTexture = LoadTexture("Assets/Outfits/Chain.bmp");
	dirtTexture = LoadTexture("Assets/Outfits/Dirt.bmp");
}

void LoadPropTextures()
{
	beltTexture = LoadTexture("Assets/Props/Belt.bmp");
}

void LoadEnvironmentTextures()
{
	skyTexture = LoadTexture("Assets/Environment/Sky.bmp");
	seaTexture = LoadTexture("Assets/Environment/Sea.bmp");
	groundTexture = LoadTexture("Assets/Environment/Ground.bmp");
}

void LoadWeaponTextures()
{
	//Spear Texture
	goldenTexture = LoadTexture("Assets/Weapon/gold.bmp");
	sliverTexture = LoadTexture("Assets/Weapon/sliver.bmp");
	spearBlade = LoadTexture("Assets/Weapon/SpearBlade.bmp");
	spearRedBlade = LoadTexture("Assets/Weapon/goldenRed.bmp");
	//Fish Texture
	fishFinTexture = LoadTexture("Assets/Weapon/FishFIn.bmp");
	fishTailTexture = LoadTexture("Assets/Weapon/FishTail.bmp");
	fishBodyTexture = LoadTexture("Assets/Weapon/FishBody.bmp");
	fishUpperBackTexture = LoadTexture("Assets/Weapon/FishUpperBack.bmp");
	fishEyeWhiteTexture = LoadTexture("Assets/Weapon/FishEyeWhite.bmp");
	fishGoldBackTexture = LoadTexture("Assets/Weapon/FishGoldBack.bmp");
	//Fire Wheel Texture
	fireCoreTexture = LoadTexture("Assets/Weapon/fireCore.bmp");
	fireOuterTexture = LoadTexture("Assets/Weapon/fireOuter.bmp");
	fireWheelBladeTexture = LoadTexture("Assets/Weapon/FireWheel_Blade.bmp");
	fireWheelRing = LoadTexture("Assets/Weapon/Firewheel_ring.bmp");
	redCoreTexture = LoadTexture("Assets/Weapon/RedCoreGlow.bmp");
	blueFireCoreTexture = LoadTexture("Assets/Weapon/BlueFireCore.bmp");
	blueFireOuterTexture = LoadTexture("Assets/Weapon/BlueFireOuter.bmp");
	//BananaLeaf
	bananaLeafTexture = LoadTexture("Assets/Weapon/BananaLeaf.bmp");
	bananaLeafHandleTexture = LoadTexture("Assets/Weapon/BananaFanHandle.bmp");
	goldBananaLeafTexture = LoadTexture("Assets/Weapon/GoldBananaLeaf.bmp");
	diamondTexture = LoadTexture("Assets/Weapon/diamond.bmp");
	//Gun
	blackSilverTexture = LoadTexture("Assets/Weapon/blackSliver.bmp");
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

void DrawPlane(float width, float depth)
{
	float w = width / 2;
	float d = depth / 2;

	float repeat = 30.0f; // Texture repeat factor

	glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal for lighting

	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, repeat); glVertex3f(-w, 0.0f, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, d);
	glTexCoord2f(repeat, 0.0f); glVertex3f(w, 0.0f, d);
	glTexCoord2f(repeat, repeat); glVertex3f(w, 0.0f, -d);
	glEnd();
	glPopMatrix();
}

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

void DrawDisk(GLUquadricObj* disk, float innerRadius, float outerRadius, int slices, int loops)
{
	glPushMatrix();
	glRotatef(-GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluDisk(disk, innerRadius, outerRadius, slices, loops);
	glPopMatrix();
}

void DrawFlatCircle(GLUquadricObj* disk, float radius, int slices, int loops)
{
	DrawDisk(disk, 0.0f, radius, slices, loops);
}

void DrawCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	glRotatef(GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);
	glPopMatrix();
}

void DrawSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);
}

void DrawEnclosedCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
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

void DrawPartialEnclosedCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
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

void DrawEnclosedSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);
}

void DrawPartialEnclosedSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawPartialEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks, isBottomClosed, isTopClosed);

	glDisable(GL_CLIP_PLANE0);
}

void DrawEnclosedCylinderWithThickness(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
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

void DrawEnclosedSemiCylinderWithThickness(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinderWithThickness(cylinder, baseRadius, topRadius, height, thickness, slices, stacks, loops);

	glDisable(GL_CLIP_PLANE0);
}

void DrawBentCylinder(GLUquadricObj* cylinder, float tubeRadius, float edgeRadius, float arcRadius, float bendAngle, int segments, float slices, float stacks)
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

void DrawEnclosedBentCylinder(GLUquadricObj* cylinder, float tubeRadius, float edgeRadius, float arcRadius, float bendAngle, int segments, float slices, float stacks)
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

void DrawSphere(GLUquadricObj* quadric, float radius, int slices, int stacks)
{
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE); // critical

	gluSphere(quadric, radius, slices, stacks);
	glPopMatrix();
}

void DrawSemiSphere(GLUquadricObj* quadric, float radius, int slices, int stacks)
{
	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
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
	case CENTER:
		SetPerspectiveProjection(60.0f, 1.0f, 0.1f, 100.0f);

		gluLookAt(
			0.0f, 0.0f, 0.0f,					// cam position (left side)
			characterX, characterY, characterZ, // character position
			0.0f, 1.0f, 0.0f					// up vector
		);

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
	}
}

void SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	SetupCameraMode();
	glMatrixMode(GL_MODELVIEW);
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

		GLfloat lightPosition[] = { lightX, lightY, lightZ, 1.0f };
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
			lightsColor[lightIndex]->b };

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
	Color scarfTint = GetElementScarfTint();
	GLfloat scarfAmbient[] = { scarfTint.r * 0.22f, scarfTint.g * 0.22f, scarfTint.b * 0.22f, 1.0f };
	GLfloat scarfDiffuse[] = { scarfTint.r * 0.95f, scarfTint.g * 0.95f, scarfTint.b * 0.95f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, scarfAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, scarfDiffuse);
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

void DrawSpear(float scale)
{
	GLUquadricObj* quad = gluNewQuadric();

	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH); // IMPORTANT

	glPushMatrix();
	glScalef(scale, scale, scale);
	glRotatef(-90, 1, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glColor3f(1.0f, 1.0f, 1.0f);

	// =========================
	// 1. HANDLE BASE
	// =========================
	glBindTexture(GL_TEXTURE_2D,
		currentBladeIndex == 0 ? goldenTexture : redCoreTexture);

	glPushMatrix();
	glTranslatef(0, 0, -0.6f);
	gluCylinder(quad, 0.04, 0.04, 0.6, 20, 1);
	glPopMatrix();

	// =========================
	// 3. SPIKE END
	// =========================
	glPushMatrix();
	glTranslatef(0, 0, -0.75f);
	gluCylinder(quad, 0.0, 0.18, 0.2, 20, 1);
	glPopMatrix();

	// =========================
	// 4. MAIN SHAFT
	// =========================
	glBindTexture(GL_TEXTURE_2D,
		currentBladeIndex == 0 ? sliverTexture : goldenTexture);

	gluCylinder(quad, 0.05, 0.05, 2.5, 20, 1);

	// =========================
	// 5. LEAF DECOR
	// =========================
	glBindTexture(GL_TEXTURE_2D,
		currentBladeIndex == 0 ? goldenTexture : redCoreTexture);

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 6; i++)
		{
			glPushMatrix();

			glTranslatef(0, 0, 2.5f + (j * 0.1f));
			glRotatef(i * 60 + (j * 30), 0, 0, 1);
			glRotatef(25, 1, 0, 0);

			glScalef(0.6f, 1.3f, 0.1f);

			// sphere already has normals via GLU
			gluSphere(quad, 0.18, 16, 16);

			glPopMatrix();
		}
	}

	// =========================
	// 6. BLADE
	// =========================
	glBindTexture(GL_TEXTURE_2D,
		currentBladeIndex == 0 ? spearBlade : spearRedBlade);

	glPushMatrix();
	glTranslatef(0, 0, 2.65f);

	// Tip
	glPushMatrix();
	gluCylinder(quad, 0.12, 0.0, 1.2, 20, 1);
	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	gluDeleteQuadric(quad);
}

void DrawWindFireWheel(float size) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL); // Always solid

	// Tell the quadric to generate texture coordinates!
	gluQuadricTexture(quad, GL_TRUE);

	// Automatically calculates smooth lighting normals for rings/spheres/cylinders!
	gluQuadricNormals(quad, GLU_SMOOTH);

	glPushMatrix();
	glScalef(size, size, size);

	// CRITICAL FIX: If you scale an object, you MUST enable normalize, 
	// otherwise the lighting normals get scaled too and shadows will break!
	glEnable(GL_NORMALIZE);

	// ==========================================
	// ANIMATION LOGIC
	// ==========================================
	wheelRotationAngle += wheelRotationSpeed;

	if (wheelRotationAngle >= 360.0f) wheelRotationAngle -= 360.0f;
	if (wheelRotationAngle <= -360.0f) wheelRotationAngle += 360.0f;

	glRotatef(wheelRotationAngle, 0.0f, 0.0f, 1.0f);
	// ==========================================

	// Turn on texturing
	glEnable(GL_TEXTURE_2D);

	// --- 1. THE MAIN RING (Bronze) ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fireWheelRing);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, goldenTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f); // Reset to white so texture shows its true color

	glPushMatrix();
	glTranslatef(0, 0, -0.05f);

	// FIX: Inner cylinder (Normals must point INSIDE towards the center hole)
	gluQuadricOrientation(quad, GLU_INSIDE);
	gluCylinder(quad, 0.7, 0.7, 0.1, 60, 1);

	// FIX: Outer cylinder (Normals must point OUTSIDE)
	gluQuadricOrientation(quad, GLU_OUTSIDE);
	gluCylinder(quad, 0.8, 0.8, 0.1, 60, 1);

	// FIX: Back Disk (Normals must point BACKWARDS in -Z)
	gluQuadricOrientation(quad, GLU_INSIDE);
	gluDisk(quad, 0.7, 0.8, 60, 1);

	glTranslatef(0, 0, 0.1f);
	// FIX: Front Disk (Normals must point FORWARDS in +Z)
	gluQuadricOrientation(quad, GLU_OUTSIDE);
	gluDisk(quad, 0.7, 0.8, 60, 1);
	glPopMatrix();

	// Reset orientation back to default for the remaining shapes
	gluQuadricOrientation(quad, GLU_OUTSIDE);

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
		if (currentBladeIndex == 0) {
			glBindTexture(GL_TEXTURE_2D, fireWheelBladeTexture);
		}
		else if (currentBladeIndex == 1) {
			glBindTexture(GL_TEXTURE_2D, fireWheelRing);
		}
		glColor3f(1.0f, 1.0f, 1.0f);

		// Front Face
		glNormal3f(0.0f, 0.0f, 1.0f); // Normal points toward the screen (+Z)
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < resolution; j++) {
			float t = (float)j / (resolution - 1); // For texture mapping
			glTexCoord2f(t, 0.0f); glVertex3f(innerX[j], innerY[j], thickness / 2);
			glTexCoord2f(t, 1.0f); glVertex3f(outerX[j], outerY[j], thickness / 2);
		}
		glEnd();

		// Back Face
		glNormal3f(0.0f, 0.0f, -1.0f); // Normal points away from the screen (-Z)
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < resolution; j++) {
			float t = (float)j / (resolution - 1);
			glTexCoord2f(t, 0.0f); glVertex3f(innerX[j], innerY[j], -thickness / 2);
			glTexCoord2f(t, 1.0f); glVertex3f(outerX[j], outerY[j], -thickness / 2);
		}
		glEnd();

		// Connecting Edges (The "Thickness")
		glBegin(GL_QUAD_STRIP); // Outer rim
		for (int j = 0; j < resolution; j++) {
			float t = (float)j / (resolution - 1);

			// Calculate smooth outward-facing normal for the edge
			float len = sqrt(outerX[j] * outerX[j] + outerY[j] * outerY[j]);
			if (len > 0.001f) glNormal3f(outerX[j] / len, outerY[j] / len, 0.0f);
			else glNormal3f(0.0f, 1.0f, 0.0f);

			glTexCoord2f(t, 0.0f); glVertex3f(outerX[j], outerY[j], thickness / 2);
			glTexCoord2f(t, 1.0f); glVertex3f(outerX[j], outerY[j], -thickness / 2);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP); // Inner rim
		for (int j = 0; j < resolution; j++) {
			float t = (float)j / (resolution - 1);

			// Calculate smooth inward-facing normal for the edge
			float len = sqrt(innerX[j] * innerX[j] + innerY[j] * innerY[j]);
			if (len > 0.001f) glNormal3f(-innerX[j] / len, -innerY[j] / len, 0.0f);
			else glNormal3f(0.0f, -1.0f, 0.0f);

			glTexCoord2f(t, 0.0f); glVertex3f(innerX[j], innerY[j], thickness / 2);
			glTexCoord2f(t, 1.0f); glVertex3f(innerX[j], innerY[j], -thickness / 2);
		}
		glEnd();

		// B. THE RED CORE GLOW
		if (currentBladeIndex == 0) {
			glBindTexture(GL_TEXTURE_2D, redCoreTexture);
		}
		else if (currentBladeIndex == 1) {
			glBindTexture(GL_TEXTURE_2D, spearRedBlade);
		}
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color
		glPushMatrix();
		glTranslatef(0.15f, 0.1f, 0.0f);
		glScalef(1.5f, 0.6f, 1.5f);
		gluSphere(quad, 0.18, 20, 20);
		glPopMatrix();

		glPopMatrix();
	}

	// ==========================================
	// --- 3. CENTER FIRE MODEL (High Speed Only) ---
	// ==========================================
	if (wheelRotationSpeed > 0.2f || wheelRotationSpeed < -0.2f) {
		glPushMatrix();

		float pulse = 1.0f + 0.15f * sin(wheelRotationAngle * 3.14159f / 180.0f * 5.0f);
		glScalef(pulse, pulse, pulse);

		// Core of the flame
		if (currentBladeIndex == 0) {
			glBindTexture(GL_TEXTURE_2D, fireCoreTexture);
		}
		else if (currentBladeIndex == 1) {
			glBindTexture(GL_TEXTURE_2D, blueFireCoreTexture);
		}
		glColor3f(1.0f, 1.0f, 1.0f);
		gluSphere(quad, 0.25, 20, 20);

		// Outer flames shooting outwards
		if (currentBladeIndex == 0) {
			glBindTexture(GL_TEXTURE_2D, fireOuterTexture);
		}
		else if (currentBladeIndex == 1) {
			glBindTexture(GL_TEXTURE_2D, blueFireOuterTexture);
		}
		glColor3f(1.0f, 1.0f, 1.0f);

		for (int i = 0; i < 4; i++) {
			glPushMatrix();
			glRotatef(i * 90.0f + (wheelRotationAngle * 1.5f), 0.0f, 0.0f, 1.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			gluCylinder(quad, 0.15, 0.0, 0.45, 12, 1);
			glPopMatrix();
		}

		glPopMatrix();
	}

	// Clean up
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_NORMALIZE); // Turn it off so it doesn't affect other objects if they don't need it
	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawFishSword(float scale) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE); // CRITICAL: Allows spheres/cylinders to use textures

	// ADD THIS: Automatically calculates smooth lighting normals for all Spheres and Cylinders!
	gluQuadricNormals(quad, GLU_SMOOTH);

	glPushMatrix();
	glScalef(scale, scale, scale);

	// Enable texturing for the whole model
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f); // Set to white so textures aren't tinted

	// --- 1. Main Fish Body ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishBodyTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, fireWheelRing);
	}
	glPushMatrix();
	glScalef(0.12f, 1.5f, 0.35f);
	gluSphere(quad, 1.0, 30, 30);
	glPopMatrix();

	// --- 2. Upper Back ---
	glBindTexture(GL_TEXTURE_2D, fishUpperBackTexture);
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishUpperBackTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, fishGoldBackTexture);
	}
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.08f);
	glScalef(0.11f, 1.45f, 0.3f);
	gluSphere(quad, 1.0, 20, 20);
	glPopMatrix();

	// --- 3. Long Tail Stalk (Handle) ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishTailTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, spearRedBlade);
	}
	glPushMatrix();
	glTranslatef(0.0f, 1.4f, 0.0f);
	glScalef(0.04f, 0.7f, 0.08f);
	gluSphere(quad, 1.0, 20, 20);
	glPopMatrix();

	// --- 4. Eyes ---
	// Right eye
	glPushMatrix();
	glTranslatef(0.07f, -1.25f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, redCoreTexture);
	gluSphere(quad, 0.05, 10, 10);

	glDisable(GL_TEXTURE_2D); // Disable for the ring
	glColor3f(1.0f, 0.8f, 0.0f);
	glRotatef(90, 0, 1, 0);
	gluCylinder(quad, 0.06, 0.06, 0.02, 10, 1);
	glEnable(GL_TEXTURE_2D); // Re-enable
	glPopMatrix();

	// Left eye
	glPushMatrix();
	glTranslatef(-0.07f, -1.25f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, redCoreTexture);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(quad, 0.05, 10, 10);

	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.8f, 0.0f);
	glRotatef(-90, 0, 1, 0);
	gluCylinder(quad, 0.06, 0.06, 0.02, 10, 1);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();

	// --- 5. Tail Fin ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishTailTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, spearRedBlade);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 1.9f, 0.0f);

	// Right Side of the Tail Fin
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	// Top Tail (Mapping texture coordinates manually for triangles)
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.5f, 0.4f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.3f, 0.0f);
	// Bottom Tail
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.5f, -0.4f);
	glEnd();

	// Left Side of the Tail Fin (Duplicated to catch light from the other side)
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	// Top Tail
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.5f, 0.4f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.3f, 0.0f);
	// Bottom Tail
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.5f, -0.4f);
	glEnd();
	glPopMatrix();

	// --- 6. Dorsal Fin ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishFinTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, spearRedBlade);
	}
	glPushMatrix();

	// Right Side of Dorsal Fin
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -0.6f, 0.25f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.6f, 0.25f);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.1f, 0.5f);
	glEnd();

	// Left Side of Dorsal Fin (Duplicated to catch light from the other side)
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -0.6f, 0.25f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.6f, 0.25f);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 0.1f, 0.5f);
	glEnd();
	glPopMatrix();

	// --- 7. Pectoral Fins ---
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, fishFinTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, spearRedBlade);
	}
	// Right
	glPushMatrix();
	glTranslatef(0.11f, -0.8f, -0.05f);
	glRotatef(-15, 0, 0, 1); glRotatef(-180, 1, 0, 0);
	glScalef(0.02f, 0.3f, 0.15f);
	glTranslatef(0.0f, -1.0f, 0.0f);
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	// Left
	glPushMatrix();
	glTranslatef(-0.11f, -0.8f, -0.05f);
	glRotatef(15, 0, 0, 1); glRotatef(-180, 1, 0, 0);
	glScalef(0.02f, 0.3f, 0.15f);
	glTranslatef(0.0f, -1.0f, 0.0f);
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawBananaLeafFan(float scale) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE); // Enable textures on cylinders/spheres

	glPushMatrix();
	glScalef(scale, scale, scale);

	// ==========================================
	// --- 1. THE HANDLE & TASSELS ---
	// ==========================================

	// Handle Base / Ring (Now Textured!)
	glEnable(GL_TEXTURE_2D); // Make sure textures are ENABLED, not disabled!
	glBindTexture(GL_TEXTURE_2D, redCoreTexture);

	glColor3f(1.0f, 1.0f, 1.0f); // Reset to white so texture shows its true colors
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.015f);
	gluCylinder(quad, 0.05, 0.05, 0.03, 20, 1);
	glPopMatrix();

	// Bottom Tassels (Now Textured!)
	glPushMatrix();
	glTranslatef(0.04f, -0.15f, 0.0f);
	glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.03f, 0.1f, 0.03f);
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.04f, -0.15f, 0.0f);
	glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.03f, 0.1f, 0.03f);
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();

	// Main Handle Wrapping 
	glEnable(GL_TEXTURE_2D);
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, bananaLeafHandleTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, sliverTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quad, 0.04, 0.04, 0.5, 15, 1);
	glPopMatrix();


	// ==========================================
	// --- 2. THE CENTRAL SPINE ---
	// ==========================================
	glEnable(GL_TEXTURE_2D);
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, sliverTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, bananaLeafHandleTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quad, 0.03, 0.015, 1.4, 10, 1);
	glPopMatrix();

	// Top Spine Tassel (Now Textured!)
	glEnable(GL_TEXTURE_2D);
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, redCoreTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, diamondTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 1.9f, 0.0f);
	glScalef(0.04f, 0.12f, 0.04f);
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();


	// ==========================================
	// --- 3. THE MAGIC LEAF (Simple Vertex Shape) ---
	// ==========================================
	glEnable(GL_TEXTURE_2D);
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, bananaLeafTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, goldBananaLeafTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	float thickness = 0.02f;
	const int NUM_POINTS = 6;

	float profileX[NUM_POINTS] = { 0.05f, 0.60f, 0.35f, 0.70f, 0.40f, 0.00f };
	float profileY[NUM_POINTS] = { 0.50f, 0.80f, 1.20f, 1.55f, 1.80f, 1.70f };

	// A. FRONT FACE 
	glNormal3f(0, 0, 1);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i < NUM_POINTS; i++) {
		float texV = (float)i / (NUM_POINTS - 1);
		glTexCoord2f(0.0f, texV); glVertex3f(-profileX[i], profileY[i], thickness);
		glTexCoord2f(1.0f, texV); glVertex3f(profileX[i], profileY[i], thickness);
	}
	glEnd();

	// B. BACK FACE
	glNormal3f(0, 0, -1);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i < NUM_POINTS; i++) {
		float texV = (float)i / (NUM_POINTS - 1);
		glTexCoord2f(1.0f, texV); glVertex3f(-profileX[i], profileY[i], -thickness);
		glTexCoord2f(0.0f, texV); glVertex3f(profileX[i], profileY[i], -thickness);
	}
	glEnd();


	// ==========================================
	// --- 4. THE BORDER RIM (Now Textured!) ---
	// ==========================================
	glEnable(GL_TEXTURE_2D); // Make sure this is ENABLED!
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, bananaLeafTexture);
	}
	else if (currentBladeIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, goldenTexture);
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	glNormal3f(1, 0, 0);
	glBegin(GL_QUAD_STRIP);
	// Trace up the Right Side
	for (int i = 0; i < NUM_POINTS; i++) {
		float texV = (float)i / (NUM_POINTS - 1);
		glTexCoord2f(0.0f, texV); glVertex3f(profileX[i], profileY[i], thickness);
		glTexCoord2f(1.0f, texV); glVertex3f(profileX[i], profileY[i], -thickness);
	}

	glNormal3f(-1, 0, 0);
	// Trace down the Left Side
	for (int i = NUM_POINTS - 1; i >= 0; i--) {
		float texV = (float)i / (NUM_POINTS - 1);
		glTexCoord2f(0.0f, texV); glVertex3f(-profileX[i], profileY[i], thickness);
		glTexCoord2f(1.0f, texV); glVertex3f(-profileX[i], profileY[i], -thickness);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawMagnumGun(float scale, bool isShooting) {
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE); // Essential for GLU shapes to show textures

	glPushMatrix();
	glScalef(scale * 0.8f, scale * 0.8f, scale * 0.8f);

	// ==========================================
	// --- 1. THE BARREL & UNDERLUG ---
	// ==========================================
	glEnable(GL_TEXTURE_2D);
	// Choose texture based on your index
	if (currentBladeIndex == 0)
		glBindTexture(GL_TEXTURE_2D, sliverTexture);
	else
		glBindTexture(GL_TEXTURE_2D, goldenTexture);

	glColor3f(1.0f, 1.0f, 1.0f); // Use white color so texture isn't tinted dark

	glPushMatrix();
	glTranslatef(-0.05f, 0.05f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

	// Main Barrel Tube
	gluCylinder(quad, 0.025, 0.025, 0.45f, 15, 1);

	// Underlug (Texture follows automatically)
	glTranslatef(0.0f, -0.015f, 0.0f);
	gluCylinder(quad, 0.02, 0.02, 0.4f, 15, 1);
	glPopMatrix();

	// ==========================================
	// --- 2. THE CYLINDER (MAGAZINE) ---
	// ==========================================
	// Use the opposite metal texture for contrast
	if (currentBladeIndex == 0)
		glBindTexture(GL_TEXTURE_2D, blackSilverTexture);
	else
		glBindTexture(GL_TEXTURE_2D, fishGoldBackTexture);

	glPushMatrix();
	glTranslatef(-0.2f, 0.03f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

	gluCylinder(quad, 0.07, 0.07, 0.15f, 15, 1);
	gluDisk(quad, 0.0, 0.07, 15, 1); // Front cap
	glTranslatef(0.0f, 0.0, 0.15f);
	gluDisk(quad, 0.0, 0.07, 15, 1); // Back cap
	glPopMatrix();

	// ==========================================
	// --- 3. THE FRAME (BODY) - Manual Texturing ---
	// ==========================================
	if (currentBladeIndex == 0) {
		glBindTexture(GL_TEXTURE_2D, sliverTexture);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, goldenTexture);
	}

	glPushMatrix();
	// Position the frame
	glTranslatef(-0.23f, 0.03f, 0.0f);

	// Call your method using the dimensions previously used in glScalef
	// width = 0.12f, height = 0.10f, depth = 0.05f
	DrawCuboidPolygon(0.12f, 0.10f, 0.05f);
	glPopMatrix();
	// Top strap (metal over the cylinder)
	glPushMatrix();
	glTranslatef(-0.25f, 0.10f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 0.2f, 1.0f);
	gluCylinder(quad, 0.04, 0.04, 0.2f, 10, 1);
	glPopMatrix();

	// ==========================================
	// --- 4. THE HANDLE (WOODEN GRIP) ---
	// ==========================================
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glPushMatrix();
	glTranslatef(-0.24f, 0.02f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 0.4f, 1.0f);
	gluCylinder(quad, 0.04, 0.04, 0.16f, 15, 1);
	glTranslatef(0.0f, 0.0f, 0.16f);
	gluDisk(quad, 0.0, 0.04, 15, 1);
	glPopMatrix();

	// ==========================================
	// --- 5. TRIGGER & GUARD ---
	// ==========================================
	glEnable(GL_TEXTURE_2D); // Re-enable texturing
	if (currentBladeIndex == 0)
		glBindTexture(GL_TEXTURE_2D, sliverTexture);
	else
		glBindTexture(GL_TEXTURE_2D, goldenTexture);

	glColor3f(1.0f, 1.0f, 1.0f); // Set to white to see the texture clearly

	// --- Trigger Guard ---
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 10; i++) {
		float angle = 3.14159f + ((float)i / 10.0f) * 3.14159f;
		float px = -0.15f + cos(angle) * 0.05f;
		float py = -0.04f + sin(angle) * 0.05f;

		// Map texture: 'u' goes from 0 to 1 across the loop
		float u = (float)i / 10.0f;
		glNormal3f(cos(angle), sin(angle), 0.0f);
		glTexCoord2f(u, 0.0f); glVertex3f(px, py, 0.005f);
		glTexCoord2f(u, 1.0f); glVertex3f(px, py, -0.005f);
	}
	glEnd();

	// --- Trigger ---
	glPushMatrix();
	glTranslatef(-0.16f, -0.06f, 0.0f);
	glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.01f, 0.03f, 0.01f);

	// gluSphere will use the bound metal texture automatically 
	// because gluQuadricTexture(quad, GL_TRUE) was called at the start
	gluSphere(quad, 1.0, 10, 10);
	glPopMatrix();


	// ==========================================
	// --- 6. SHOOTING EFFECT (Flash & Trail) ---
	// ==========================================
	if (isShooting) {
		glPushMatrix();

		// Move to the exact tip of the barrel
		glTranslatef(0.40f, 0.05f, 0.0f);

		// Turn on blending for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING); // Makes the fire glow brightly

		// --- Muzzle Flash (Orange & Yellow Sphere) ---
		// Outer orange glow
		glColor4f(1.0f, 0.5f, 0.0f, 0.8f); // 80% opaque
		glPushMatrix();
		glScalef(0.05f, 0.05f, 0.05f);
		gluSphere(quad, 1.0, 10, 10);
		glPopMatrix();

		// Inner yellow bright flash
		glColor4f(1.0f, 1.0f, 0.2f, 1.0f); // 100% solid
		glPushMatrix();
		glScalef(0.03f, 0.03f, 0.03f);
		gluSphere(quad, 1.0, 10, 10);
		glPopMatrix();

		// --- Trajectory (Transparent Cylinder) ---
		// Point the trail forward along the X axis
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

		// Light yellow/white transparent beam
		glColor4f(1.0f, 0.9f, 0.5f, 0.5f); // 50% transparent

		// Very thin cylinder, stretching super far forward (length 5.0f)
		gluCylinder(quad, 0.005, 0.005, 5.0f, 10, 1);

		glEnable(GL_LIGHTING); // Turn lighting back on
		glDisable(GL_BLEND);   // Turn off blending
		glPopMatrix();
	}

	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawKiritoInspiredSword(bool isMainSword, float scale)
{
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE);

	glPushMatrix();
	glScalef(scale, scale, scale);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	float bladeLength = isMainSword ? 1.55f : 1.25f;
	float bladeWidth = isMainSword ? 0.16f : 0.13f;
	float bladeThick = 0.045f;
	float gripCenterZ = 0.28f;

	// Re-anchor the model so the grip sits at the local origin.
	// This makes hand placement much easier and keeps the handle aligned with the palm.
	glTranslatef(0.0f, 0.0f, gripCenterZ);

	glBindTexture(GL_TEXTURE_2D, isMainSword ? steelTexture : silverTexture);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, bladeLength * 0.5f);
	DrawCuboidPolygon(bladeWidth, bladeThick, bladeLength);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, currentBladeIndex == 0 ? spearBlade : spearRedBlade);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, bladeLength * 0.42f);
	DrawCuboidPolygon(bladeWidth * 0.35f, bladeThick * 0.45f, bladeLength * 0.78f);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, currentBladeIndex == 0 ? spearBlade : spearRedBlade);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, bladeLength + 0.12f);
	glScalef(bladeWidth * 0.9f, bladeThick, 0.24f);
	DrawPyramid(1.0f, 1.0f);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, isMainSword ? goldenTexture : silverTexture);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.04f);
	DrawCuboidPolygon(0.22f, 0.07f, 0.12f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.04f);
	DrawCuboidPolygon(isMainSword ? 0.42f : 0.32f, 0.035f, 0.07f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.12f, 0.0f, 0.02f);
	glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
	DrawCuboidPolygon(0.08f, 0.03f, 0.12f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.12f, 0.0f, 0.02f);
	glRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
	DrawCuboidPolygon(0.08f, 0.03f, 0.12f);
	glPopMatrix();

	float gripLength = isMainSword ? 0.42f : 0.36f;

	glBindTexture(GL_TEXTURE_2D, brownClothTexture);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.28f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quad, 0.035f, 0.03f, gripLength, 16, 8);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, chainTexture);
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.12f - i * 0.08f);
		DrawCuboidPolygon(0.09f, 0.012f, 0.025f);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, isMainSword ? steelTexture : silverTexture);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.50f);
	DrawSphere(quad, isMainSword ? 0.05f : 0.045f, 16, 16);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	gluDeleteQuadric(quad);
}

void DrawDualSwordInHand(float side)
{
	glPushMatrix();

	if (side > 0.0f)
	{
		glTranslatef(-0.018f, 0.004f, 0.010f);
		glScalef(0.18f, 0.18f, 0.18f);
		glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-3.0f, 1.0f, 0.0f, 0.0f);
		DrawKiritoInspiredSword(true, 0.75f);
	}
	else
	{
		glTranslatef(-0.018f, -0.004f, 0.012f);
		glScalef(0.17f, 0.17f, 0.17f);
		glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-0.0f, 1.0f, 0.0f, 0.0f);
		DrawKiritoInspiredSword(false, 0.72f);
	}

	glPopMatrix();
}


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

void DrawFinger(float length, float bendAngle)
{
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

void DrawHand(float side)
{
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
	if ((currentSceneMode == ANIMATION && currentAnimType != 6 && currentAnimType != 7 && currentAnimType != 8)
		|| (currentLeftWeapon != 0 && currentLeftWeapon != 6)
		|| (currentRightWeapon != 0 && currentRightWeapon != 6))
		glRotatef(side * -100.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-70.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip! (Multiplied by 0.5 so thumb bends a bit less)
	DrawFinger(thumbLength, currentGrip * 0.5f);
	glPopMatrix();
	// END Thumb Finger

	// Index Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + indexLength / 3), 0.0f, fingerSpacing);
	if ((currentSceneMode == ANIMATION && currentAnimType != 6 && currentAnimType != 7 && currentAnimType != 8)
		|| (currentLeftWeapon != 0 && currentLeftWeapon != 6)
		|| (currentRightWeapon != 0 && currentRightWeapon != 6))
		glRotatef(side * -90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(indexLength, currentGrip);
	glPopMatrix();
	// END Index Finger

	// Middle Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + middleLength / 2), 0.0f, 0.0f);
	if ((currentSceneMode == ANIMATION && currentAnimType != 6 && currentAnimType != 7 && currentAnimType != 8)
		|| (currentLeftWeapon != 0 && currentLeftWeapon != 6)
		|| (currentRightWeapon != 0 && currentRightWeapon != 6))
		glRotatef(side * -90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(middleLength, currentGrip);
	glPopMatrix();
	// END Middle Finger

	// Ring Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + ringLength / 3), 0.0f, -fingerSpacing);
	if ((currentSceneMode == ANIMATION && currentAnimType != 6 && currentAnimType != 7 && currentAnimType != 8)
		|| (currentLeftWeapon != 0 && currentLeftWeapon != 6)
		|| (currentRightWeapon != 0 && currentRightWeapon != 6))
		glRotatef(side * -90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(ringLength, currentGrip);
	glPopMatrix();
	// END Ring Finger

	// Little Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2), 0.0f, -fingerSpacing * 2);
	if ((currentSceneMode == ANIMATION && currentAnimType != 6 && currentAnimType != 7 && currentAnimType != 8) 
		|| (currentLeftWeapon != 0 && currentLeftWeapon != 6) 
		|| (currentRightWeapon != 0 && currentRightWeapon != 6))
		glRotatef(side * -90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	// Added currentGrip!
	DrawFinger(littleLength, currentGrip);
	glPopMatrix();
	// END Little Finger

	glPopMatrix();
	// END Palm

	glDisable(GL_TEXTURE_2D);
}

void DrawArm(float side)
{
	float upperArmLength = 0.06f;
	float lowerArmLength = 0.08f;

	glRotatef((side + 1.0f) * 90.0f, 0.0f, 0.0f, 1.0f);

	// Upper Arm
	glPushMatrix();
	PartRotation& upperArm =
		(side < 0) ? parts[LEFT_UPPER_ARM] : parts[RIGHT_UPPER_ARM];
	glRotatef(upperArm.angleX, 1, 0, 0);
	glRotatef(upperArm.angleY, 0, 1, 0);
	glRotatef(upperArm.angleZ, 0, 0, 1);
	DrawUpperArm(upperArmLength);

	// Lower Arm
	glPushMatrix();
	glTranslatef(-(upperArmLength / 2 + lowerArmLength / 2), 0.0f, 0.0f);
	PartRotation& lowerArm =
		(side < 0) ? parts[LEFT_LOWER_ARM] : parts[RIGHT_LOWER_ARM];
	glRotatef(lowerArm.angleX, 1, 0, 0);
	glRotatef(lowerArm.angleY, 0, 1, 0);
	glRotatef(lowerArm.angleZ, 0, 0, 1);
	DrawLowerArm(lowerArmLength);

	// Hand
	glPushMatrix();
	glTranslatef(-lowerArmLength, 0.0f, 0.0f);
	PartRotation& hand =
		(side < 0) ? parts[LEFT_HAND] : parts[RIGHT_HAND];
	glRotatef(hand.angleX, 1, 0, 0);
	glRotatef(hand.angleY, 0, 1, 0);
	glRotatef(hand.angleZ, 0, 0, 1);
	DrawHand(side);

	// ==========================================
	// NEW WEAPON LOGIC (Works for both sides)
	// ==========================================
	// Determine which weapon index to use
	int handWeapon = (side > 0.0f) ? (currentRightWeapon % 6) : (currentLeftWeapon % 6);

	if (handWeapon != 0) {
		glPushMatrix();

		switch (handWeapon) {
		case 1: // --- SPEAR ---
			// 1. Position: X is slightly negative to align with the palm center,
			// Y is 0.0f to center it, Z pushes it slightly forward into the fingers.
			glScalef(1.0f, side, 1.0f);
			glTranslatef(-0.005f, -0.06f, -0.18f);

			// 2. Scale
			glScalef(0.2f, 0.2f, 0.2f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, -0.35f);
			DrawSpear(0.7);
			break;

		case 2: // --- WEAPON 2 (Fish Sword) ---
			// 1. Position: X is slightly negative to align with the palm center, 
			// Y is 0.0f to center it, Z pushes it slightly forward into the fingers.
			glTranslatef(-0.075f, side * 0.01f, 0.25f);
			glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.2f, 0.2f, 0.2f);
			glTranslatef(0.0f, 0.0f, -0.35f);
			DrawFishSword(0.7);
			break;

		case 3: // --- LEAF FAN ---
			glTranslatef(0.0f, side * 0.009f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(0.15f, 0.15f, 0.15f);
			DrawBananaLeafFan(1.0f);
			break;

		case 4: // --- MAGNUM GUN ---
			glTranslatef(-0.055f, side * 0.008f, 0.008f);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(0.25f, 0.25f, 0.25f);
			// If dual wielding, both guns will flash
			DrawMagnumGun(1.0f, isGunShooting);
			break;

		case 5: // Kirito Sword
			glTranslatef(0.015f, side * 0.007f, 0.0f);
			DrawDualSwordInHand(side);
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

	// NEW: Wheels appear on states 5, 6, 7, 8, and 9!
	if (currentRightWeapon >= 6 && currentRightWeapon <= 12) {
		glDisable(GL_TEXTURE_2D); // Turn off skin texture for the fiery colors

		glPushMatrix();
		// 1. Position: Y moves it down under the sole, Z pushes it forward to center under the midfoot
		glTranslatef(0.0f, -0.1f, 0.04f);

		// 2. Rotate it so it stands upright and rolls forward
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

		// 3. Draw the wheel
		DrawWindFireWheel(0.04f);
		glPopMatrix();

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

	PartRotation& upperLeg =
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
	PartRotation& lowerLeg =
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
	PartRotation& foot =
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
	bool isChibi = (currentCharacterMode == CHIBI_MODE);

	glPushMatrix();

	if (isChibi)
	{
		// smaller, slimmer handsome face
		glScalef(0.92f, 0.95f, 0.92f);
	}

	DrawHeadBase(headBaseRadius, headBaseHeight);
	DrawUpperHead();
	DrawLowerHead();

	// Eyes
	glPushMatrix();
	if (isChibi)
	{
		glTranslatef(0.0f, -0.003f, 0.0f);
		glScalef(0.88f, 0.88f, 1.0f);
	}
	DrawLeftEye();
	DrawRightEye();
	glPopMatrix();
	// END Eyes

	// Eyelashes
	glPushMatrix();
	if (isChibi)
	{
		glTranslatef(0.0f, -0.003f, 0.0f);
		glScalef(0.90f, 0.90f, 1.0f);
	}
	DrawLeftEyelash();
	DrawRightEyelash();
	glPopMatrix();
	// END Eyelashes

	// Mouth
	glPushMatrix();
	if (isChibi)
	{
		glTranslatef(0.0f, -0.006f, 0.006f);
		glScalef(0.78f, 0.78f, 1.0f);
	}
	DrawMouth();
	glPopMatrix();
	// END Mouth

	// Left Ear
	glPushMatrix();
	glTranslatef(-headBaseRadius * 1.2f, 0.0f, 0.0f);
	DrawLeftEar();

	// Left Ear Ring
	glPushMatrix();
	if (isChibi)
		glTranslatef(-0.018f, 0.006f, 0.0f);
	DrawEarRing(-1.0f);
	glPopMatrix();
	// END Left Ear Ring

	glPopMatrix();
	// END Left Ear

	// Right Ear
	glPushMatrix();
	glTranslatef(headBaseRadius * 1.2f, 0.0f, 0.0f);
	DrawRightEar();
	if (isChibi)
		glTranslatef(0.018f, 0.006f, 0.0f);
	DrawEarRing(1.0f);
	glPopMatrix();
	// END Right Ear

	// Nose
	glPushMatrix();
	if (isChibi)
	{
		glTranslatef(0.0f, -0.001f, 0.010f);
		glScalef(0.88f, 0.88f, 0.88f);
	}
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

// *********************************
// CUSTOMIZATION FUNCTIONS [EFFECTS]
// *********************************

void DrawWaterElementHeadJewel(float headBaseRadius)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.88f, 0.76f, 0.38f });

	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.86f, headBaseRadius * 1.02f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.12f, headBaseRadius * 0.12f, headBaseRadius * 0.04f, 0.18f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.68f, headBaseRadius * 0.98f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, headBaseRadius * 0.06f, headBaseRadius * 0.06f, headBaseRadius * 0.03f, 0.2f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({ 0.42f, 0.78f, 1.0f });

	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.68f, headBaseRadius * 1.03f);
	glScalef(0.75f, 0.95f, 0.45f);
	DrawSphere(quadric, headBaseRadius * 0.10f, SLICES, STACKS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.52f, headBaseRadius * 0.98f);
	glScalef(0.14f, 0.9f, 0.14f);
	DrawCuboidPolygon(headBaseRadius * 0.10f, headBaseRadius * 0.26f, headBaseRadius * 0.08f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementFrontRobe(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({ 0.22f, 0.34f, 0.46f });

	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 0.10f, torsoRadius * 0.96f);
	glScalef(0.92f, 1.55f, 0.14f);
	DrawCuboidPolygon(torsoRadius * 1.0f, torsoHeight * 2.0f, torsoRadius * 0.18f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-torsoRadius * 0.58f, torsoHeight * 0.02f, torsoRadius * 0.94f);
	glRotatef(-16.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.34f, 1.10f, 0.12f);
	DrawCuboidPolygon(torsoRadius * 0.14f, torsoHeight * 1.10f, torsoRadius * 0.08f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(torsoRadius * 0.58f, torsoHeight * 0.02f, torsoRadius * 0.94f);
	glRotatef(16.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.34f, 1.10f, 0.12f);
	DrawCuboidPolygon(torsoRadius * 0.14f, torsoHeight * 1.10f, torsoRadius * 0.08f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementGoldTrim(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.88f, 0.76f, 0.36f });

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.54f, torsoRadius * 1.28f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.28f, torsoRadius * 0.28f, torsoRadius * 0.05f, 0.16f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * torsoRadius * 1.00f, torsoHeight * 0.46f, torsoRadius * 1.18f);
		glRotatef((float)(-i * 20), 0.0f, 0.0f, 1.0f);
		glScalef(0.72f, 0.55f, 0.42f);
		DrawSemiSphere(quadric, torsoRadius * 0.18f, SLICES, STACKS);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.04f, torsoRadius * 1.30f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.11f, torsoRadius * 0.11f, torsoRadius * 0.04f, 0.24f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementGem(float torsoRadius, float torsoHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	ApplyTint({ 0.48f, 0.82f, 1.0f });

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * 0.18f, torsoRadius * 1.42f);
	glScalef(0.46f, 0.82f, 0.12f);
	DrawCuboidPolygon(torsoRadius * 0.10f, torsoHeight * 0.30f, torsoRadius * 0.06f);
	glPopMatrix();

	ResetTint();
	glDisable(GL_TEXTURE_2D);
}

void DrawWaterElementDecorations(float torsoRadius, float torsoHeight, bool isChibi)
{
	DrawWaterElementFrontRobe(torsoRadius, torsoHeight);
	DrawWaterElementGoldTrim(torsoRadius, torsoHeight);
	DrawWaterElementGem(torsoRadius, torsoHeight);
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

void DrawStylizedFireOuterShape()
{
	glBegin(GL_POLYGON);
	glVertex3f(0.00f, -0.56f, 0.0f);
	glVertex3f(-0.14f, -0.55f, 0.0f);
	glVertex3f(-0.30f, -0.48f, 0.0f);
	glVertex3f(-0.44f, -0.34f, 0.0f);
	glVertex3f(-0.52f, -0.12f, 0.0f);
	glVertex3f(-0.52f, 0.10f, 0.0f);
	glVertex3f(-0.45f, 0.30f, 0.0f);
	glVertex3f(-0.33f, 0.46f, 0.0f);
	glVertex3f(-0.20f, 0.28f, 0.0f);
	glVertex3f(-0.11f, 0.12f, 0.0f);
	glVertex3f(-0.05f, 0.26f, 0.0f);
	glVertex3f(-0.06f, 0.52f, 0.0f);
	glVertex3f(0.02f, 0.84f, 0.0f);
	glVertex3f(0.12f, 1.04f, 0.0f);
	glVertex3f(0.06f, 0.80f, 0.0f);
	glVertex3f(0.14f, 0.60f, 0.0f);
	glVertex3f(0.29f, 0.42f, 0.0f);
	glVertex3f(0.42f, 0.22f, 0.0f);
	glVertex3f(0.50f, -0.02f, 0.0f);
	glVertex3f(0.52f, -0.22f, 0.0f);
	glVertex3f(0.44f, -0.40f, 0.0f);
	glVertex3f(0.28f, -0.52f, 0.0f);
	glVertex3f(0.12f, -0.56f, 0.0f);
	glEnd();
}

void DrawStylizedFireMiddleShape()
{
	glBegin(GL_POLYGON);
	glVertex3f(0.00f, -0.56f, 0.0f);
	glVertex3f(-0.10f, -0.54f, 0.0f);
	glVertex3f(-0.20f, -0.46f, 0.0f);
	glVertex3f(-0.26f, -0.28f, 0.0f);
	glVertex3f(-0.24f, -0.04f, 0.0f);
	glVertex3f(-0.15f, 0.18f, 0.0f);
	glVertex3f(-0.07f, 0.06f, 0.0f);
	glVertex3f(-0.01f, -0.06f, 0.0f);
	glVertex3f(0.03f, 0.12f, 0.0f);
	glVertex3f(0.10f, 0.42f, 0.0f);
	glVertex3f(0.18f, 0.66f, 0.0f);
	glVertex3f(0.10f, 0.48f, 0.0f);
	glVertex3f(0.14f, 0.24f, 0.0f);
	glVertex3f(0.28f, 0.02f, 0.0f);
	glVertex3f(0.34f, -0.22f, 0.0f);
	glVertex3f(0.32f, -0.40f, 0.0f);
	glVertex3f(0.20f, -0.52f, 0.0f);
	glVertex3f(0.08f, -0.56f, 0.0f);
	glEnd();
}

void DrawStylizedFireInnerShape()
{
	glBegin(GL_POLYGON);
	glVertex3f(0.00f, -0.56f, 0.0f);
	glVertex3f(-0.08f, -0.52f, 0.0f);
	glVertex3f(-0.14f, -0.40f, 0.0f);
	glVertex3f(-0.15f, -0.18f, 0.0f);
	glVertex3f(-0.11f, 0.02f, 0.0f);
	glVertex3f(-0.05f, -0.06f, 0.0f);
	glVertex3f(-0.01f, -0.16f, 0.0f);
	glVertex3f(0.02f, 0.00f, 0.0f);
	glVertex3f(0.08f, 0.20f, 0.0f);
	glVertex3f(0.14f, 0.38f, 0.0f);
	glVertex3f(0.08f, 0.24f, 0.0f);
	glVertex3f(0.12f, 0.06f, 0.0f);
	glVertex3f(0.18f, -0.14f, 0.0f);
	glVertex3f(0.20f, -0.34f, 0.0f);
	glVertex3f(0.16f, -0.48f, 0.0f);
	glVertex3f(0.08f, -0.56f, 0.0f);
	glEnd();
}

void DrawFireFlame(float x, float y, float z, float scale, float sway, float stretch)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(sway, 0.0f, 0.0f, 1.0f);
	glScalef(scale * 0.28f, scale * stretch * 0.32f, scale * 0.55f);

	glColor4f(0.84f, 0.20f, 0.21f, 0.92f);
	DrawStylizedFireOuterShape();

	glPushMatrix();
	glTranslatef(0.00f, -0.02f, 0.01f);
	glScalef(0.72f, 0.78f, 1.0f);
	glColor4f(1.0f, 0.53f, 0.28f, 0.95f);
	DrawStylizedFireMiddleShape();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.00f, -0.02f, 0.02f);
	glScalef(0.46f, 0.56f, 1.0f);
	glColor4f(0.98f, 0.82f, 0.36f, 0.96f);
	DrawStylizedFireInnerShape();
	glPopMatrix();

	glPopMatrix();
}

void DrawFireElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != FIRE_ELEMENT)
		return;

	float currentTime = (float)(GetTickCount64() % 0xFFFFFFFF) * 0.001f;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0.30f, 0.04f, 0.02f, 0.95f);
	glPushMatrix();
	glTranslatef(0.0f, 0.2f, -2.18f);
	glScalef(1.0f, 1.05f, 0.12f);
	DrawCuboidPolygon(1.92f, 1.10f, 0.18f);
	glPopMatrix();

	for (int i = 0; i < 9; i++)
	{
		float column = -0.78f + (float)i * 0.19f;
		float bob = sinf(currentTime * (1.6f + i * 0.06f) + i * 0.55f) * 0.028f;
		float sway = sinf(currentTime * (2.0f + i * 0.10f) + i * 0.7f) * 4.0f;
		float stretch = 0.98f + 0.16f * (0.5f + 0.5f * sinf(currentTime * (1.8f + i * 0.11f) + i));
		float scale = 0.92f + (float)(i % 3) * 0.05f;
		DrawFireFlame(column, -0.16f + bob, -2.02f - (float)(i % 2) * 0.02f, scale, sway, stretch);
	}

	for (int i = 0; i < 5; i++)
	{
		float column = -0.52f + (float)i * 0.26f;
		float bob = sinf(currentTime * (2.2f + i * 0.10f) + i * 0.9f) * 0.016f;
		float sway = sinf(currentTime * (2.6f + i * 0.14f) + i * 0.4f) * 5.0f;
		float stretch = 0.92f + 0.12f * (0.5f + 0.5f * sinf(currentTime * (2.4f + i * 0.16f) + i * 0.8f));
		float scale = 0.48f + (float)(i % 2) * 0.04f;
		DrawFireFlame(column, 0.14f + bob, -1.96f, scale, sway, stretch);
	}

	for (int i = 0; i < 24; i++)
	{
		float phase = (float)i * 0.37f;
		float drift = fmodf(currentTime * (0.42f + (float)(i % 5) * 0.05f) + phase, 1.45f);
		float emberY = -0.34f + drift * 0.88f;
		float emberX = -0.82f + (float)(i % 12) * 0.14f + sinf(currentTime * (1.6f + (float)(i % 4) * 0.15f) + phase) * 0.03f;
		float emberZ = -1.94f + (float)(i % 3) * 0.02f;
		float emberScale = 0.010f + (float)(i % 4) * 0.003f;

		glColor4f(1.0f, 0.70f, 0.18f, 0.55f);
		glPushMatrix();
		glTranslatef(emberX, emberY, emberZ);
		glScalef(1.0f, 1.35f, 1.0f);
		DrawSphere(quadric, emberScale, 10, 10);
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	ResetMaterial();
}

void DrawWaterElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != WATER_ELEMENT)
		return;

	// Prevents overflow issues for long-running sessions
	float currentTime = (float)(GetTickCount64() % 0xFFFFFFFF) * 0.001f;

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

void DrawMetalElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != METAL_ELEMENT)
		return;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, steelTexture);
	ApplyTint({ 0.78f, 0.82f, 0.88f });

	glPushMatrix();
	glTranslatef(0.0f, -0.04f, -2.18f);
	glScalef(1.0f, 1.05f, 0.12f);
	DrawCuboidPolygon(1.85f, 1.12f, 0.18f);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.88f, 0.76f, 0.34f });

	glPushMatrix();
	glTranslatef(0.0f, 0.30f, -2.10f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, 0.17f, 0.17f, 0.05f, 0.18f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef((float)i * 0.44f, 0.18f, -2.08f);
		glRotatef((float)(i * 18), 0.0f, 0.0f, 1.0f);
		glScalef(1.0f, 0.72f, 0.22f);
		DrawCuboidPolygon(0.13f, 0.52f, 0.08f);
		glPopMatrix();

		glPushMatrix();
		glTranslatef((float)i * 0.62f, -0.08f, -2.06f);
		glScalef(0.75f, 0.75f, 0.28f);
		DrawSemiSphere(quadric, 0.12f, SLICES, STACKS);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, silverTexture);
	ApplyTint({ 0.96f, 0.97f, 1.0f });

	for (int row = 0; row < 2; row++)
	{
		for (int col = -2; col <= 2; col++)
		{
			glPushMatrix();
			glTranslatef((float)col * 0.26f, -0.12f - row * 0.26f, -2.04f);
			glScalef(0.42f, 0.42f, 0.12f);
			DrawCuboidPolygon(0.16f, 0.16f, 0.05f);
			glPopMatrix();
		}
	}

	ResetTint();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	ResetMaterial();
}

void DrawEarthElementBackgroundEffect()
{
	if (currentCharacterTexturePresetIndex != EARTH_ELEMENT)
		return;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, dirtTexture);
	ApplyTint({ 0.62f, 0.46f, 0.26f });

	glPushMatrix();
	glTranslatef(0.0f, -0.56f, -2.15f);
	glScalef(1.0f, 0.55f, 0.20f);
	DrawCuboidPolygon(1.95f, 0.42f, 0.22f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -0.12f, -2.18f);
	glScalef(1.0f, 0.82f, 0.12f);
	DrawCuboidPolygon(1.75f, 0.78f, 0.16f);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, brickTexture);
	ApplyTint({ 0.72f, 0.58f, 0.38f });

	for (int i = -1; i <= 1; i++)
	{
		glPushMatrix();
		glTranslatef((float)i * 0.52f, 0.06f, -2.06f);
		glScalef(0.34f, 0.92f, 0.18f);
		DrawCuboidPolygon(0.28f, 0.52f, 0.12f);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(-0.82f, -0.08f, -2.08f);
	glScalef(0.26f, 1.10f, 0.18f);
	DrawCuboidPolygon(0.18f, 0.62f, 0.12f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.82f, -0.08f, -2.08f);
	glScalef(0.26f, 1.10f, 0.18f);
	DrawCuboidPolygon(0.18f, 0.62f, 0.12f);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, goldTexture);
	ApplyTint({ 0.86f, 0.74f, 0.34f });

	glPushMatrix();
	glTranslatef(0.0f, 0.28f, -2.02f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinderWithThickness(quadric, 0.12f, 0.12f, 0.04f, 0.22f, SLICES, STACKS, LOOPS);
	glPopMatrix();

	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef((float)i * 0.32f, 0.26f, -2.01f);
		glScalef(0.40f, 0.60f, 0.16f);
		DrawCuboidPolygon(0.12f, 0.20f, 0.06f);
		glPopMatrix();
	}

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

		glPushMatrix();
		glTranslatef(0.0f, torsoHeight * 0.3f, torsoRadius * 0.5f);
		glScalef(0.9f, isChibi ? 0.75f : 1.0f, 0.18f);
		DrawCuboidPolygon(torsoRadius * 0.2f, torsoHeight * 1.35f, torsoRadius * 0.08f);
		glPopMatrix();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, goldTexture);

		glPushMatrix();
		glTranslatef(0.0f, -torsoHeight * 0.675f, torsoRadius * 0.6f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		DrawEnclosedCylinderWithThickness(quadric, torsoRadius * 0.22f, torsoRadius * 0.22f, torsoRadius * 0.08f, 0.35f, SLICES, STACKS, LOOPS);
		glPopMatrix();

		break;

	case WATER_ELEMENT:
		DrawWaterElementGoldTrim(torsoRadius, torsoHeight);
		DrawWaterElementGem(torsoRadius, torsoHeight);
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

void DrawEnergyChargeOrb(float radius, float pulse)
{
	glColor4f(0.08f, 0.34f, 1.0f, 0.22f + pulse * 0.08f);
	glPushMatrix();
	glScalef(1.95f, 1.95f, 1.95f);
	DrawSphere(quadric, radius, 18, 18);
	glPopMatrix();

	glColor4f(0.18f, 0.72f, 1.0f, 0.42f + pulse * 0.14f);
	glPushMatrix();
	glScalef(1.30f, 1.30f, 1.30f);
	DrawSphere(quadric, radius, 18, 18);
	glPopMatrix();

	glColor4f(0.28f, 0.82f, 1.0f, 0.82f);
	DrawSphere(quadric, radius, 18, 18);

	glColor4f(0.90f, 0.98f, 1.0f, 0.96f);
	DrawSphere(quadric, radius * 0.48f, 16, 16);
}

void DrawEnergyHaloRing(float radius, float thickness, float alpha, float rotX, float rotY, float rotZ)
{
	const int segments = 48;

	glPushMatrix();
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= segments; ++i)
	{
		float angle = ((float)i / (float)segments) * PI * 2.0f;
		float cs = cosf(angle);
		float sn = sinf(angle);

		glColor4f(0.10f, 0.45f, 1.0f, 0.0f);
		glVertex3f(cs * (radius + thickness), sn * (radius + thickness), 0.0f);

		glColor4f(0.56f, 0.90f, 1.0f, alpha);
		glVertex3f(cs * radius, sn * radius, 0.0f);
	}
	glEnd();

	glPopMatrix();
}

void DrawEnergyBeamSpiral(float beamLength, float beamRadius, float twistOffset, float alpha)
{
	const int segments = 36;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= segments; ++i)
	{
		float u = (float)i / (float)segments;
		float angle = twistOffset + u * PI * 5.0f;
		float radius = beamRadius * (1.6f - u * 0.6f);

		glColor4f(0.38f, 0.86f, 1.0f, alpha * (1.0f - u));
		glVertex3f(cosf(angle) * radius, sinf(angle) * radius, beamLength * u);
	}
	glEnd();
}

void DrawEnergyLaserBeam(float beamLength, float beamRadius, float pulse, float twistOffset)
{
	float haloRadius = beamRadius * (2.4f + pulse * 0.35f);
	float midRadius = beamRadius * (1.30f + pulse * 0.12f);
	float coreRadius = beamRadius * 0.46f;

	glColor4f(0.10f, 0.42f, 1.0f, 0.18f + pulse * 0.05f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, beamLength * 0.5f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawCylinder(quadric, haloRadius, beamRadius * 1.55f, beamLength, 20, 8);
	glPopMatrix();

	glColor4f(0.24f, 0.76f, 1.0f, 0.42f + pulse * 0.12f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, beamLength * 0.5f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawCylinder(quadric, midRadius, beamRadius * 0.82f, beamLength, 18, 8);
	glPopMatrix();

	glColor4f(0.92f, 0.99f, 1.0f, 0.92f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, beamLength * 0.5f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawCylinder(quadric, coreRadius, beamRadius * 0.22f, beamLength, 16, 8);
	glPopMatrix();

	glLineWidth(2.0f);
	DrawEnergyBeamSpiral(beamLength, beamRadius, twistOffset, 0.60f + pulse * 0.14f);
	DrawEnergyBeamSpiral(beamLength, beamRadius, twistOffset + PI, 0.48f + pulse * 0.10f);
	glLineWidth(1.0f);

	glColor4f(0.22f, 0.74f, 1.0f, 0.68f + pulse * 0.10f);
	DrawSphere(quadric, beamRadius * 2.1f, 18, 18);

	glColor4f(0.95f, 0.99f, 1.0f, 0.98f);
	DrawSphere(quadric, beamRadius * 0.9f, 16, 16);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, beamLength);
	glColor4f(0.34f, 0.82f, 1.0f, 0.42f + pulse * 0.10f);
	DrawSphere(quadric, beamRadius * 2.8f, 18, 18);
	glColor4f(0.96f, 1.0f, 1.0f, 0.94f);
	DrawSphere(quadric, beamRadius * 1.1f, 16, 16);
	glPopMatrix();
}

void DrawEnergyBeamAnimationEffect()
{
	if (currentSceneMode != ANIMATION || currentAnimType != 8)
		return;

	const float maxFrames = 90.0f;
	float t = Clamp(animFrame / maxFrames, 0.0f, 1.0f);
	float effectPulse = 0.5f + 0.5f * sinf(t * PI * 16.0f);
	float swirlAngle = t * 720.0f;
	float handOffsetZ = (t <= 0.60f) ? 0.16f : (0.16f + (t - 0.60f) / 0.22f * 0.18f);
	handOffsetZ = Clamp(handOffsetZ, 0.16f, 0.34f);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glPushMatrix();
	glTranslatef(characterX, characterY, characterZ);
	glTranslatef(0.0f, 0.12f, handOffsetZ);

	if (t <= 0.60f)
	{
		float phaseT = (t <= 0.35f) ? (t / 0.35f) : ((t - 0.35f) / 0.25f);
		float grow = (t <= 0.35f) ? phaseT : 1.0f;
		float baseRadius = (t <= 0.35f)
			? (0.025f + 0.12f * phaseT)
			: (0.145f + sinf(phaseT * PI * 6.0f) * 0.014f);
		float pulse = (t <= 0.35f) ? (0.30f + phaseT * 0.70f) : (0.95f + fabsf(sinf(phaseT * PI * 8.0f)) * 0.30f);

		glPushMatrix();
		glScalef(1.0f + effectPulse * 0.05f, 1.0f + effectPulse * 0.05f, 1.0f + effectPulse * 0.08f);
		DrawEnergyChargeOrb(baseRadius, pulse);
		glPopMatrix();

		DrawEnergyHaloRing(baseRadius * (1.65f + effectPulse * 0.10f), baseRadius * 0.55f, 0.36f * grow, 70.0f, swirlAngle, 0.0f);
		DrawEnergyHaloRing(baseRadius * (1.95f + effectPulse * 0.08f), baseRadius * 0.42f, 0.26f * grow, 20.0f, 90.0f + swirlAngle * 0.7f, 35.0f);
		DrawEnergyHaloRing(baseRadius * (1.25f + effectPulse * 0.05f), baseRadius * 0.24f, 0.44f * grow, 90.0f, 0.0f, -swirlAngle * 1.4f);

		if (t > 0.35f)
		{
			float jitter = sinf(phaseT * PI * 24.0f) * 0.012f;
			for (int i = 0; i < 4; ++i)
			{
				float angle = swirlAngle * 0.04f + i * (PI * 0.5f);
				float sparkRadius = baseRadius * (1.35f + 0.12f * (float)i);

				glColor4f(0.42f, 0.88f, 1.0f, 0.32f);
				glBegin(GL_LINES);
				glVertex3f(cosf(angle) * sparkRadius, sinf(angle) * sparkRadius, -0.01f);
				glVertex3f(cosf(angle) * (sparkRadius + 0.05f), sinf(angle) * (sparkRadius + 0.05f), 0.05f + jitter);
				glEnd();
			}
		}
	}
	else if (t <= 0.82f)
	{
		float phaseT = (t - 0.60f) / 0.22f;
		float ease = (1.0f - cosf(phaseT * PI)) * 0.5f;
		float beamLength = 0.45f + 2.65f * ease;
		float beamRadius = 0.055f + 0.030f * ease;
		float pulse = 0.75f + 0.25f * sinf(phaseT * PI * 10.0f);

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.04f);
		DrawEnergyLaserBeam(beamLength, beamRadius, pulse, phaseT * PI * 2.5f);
		glPopMatrix();

		float backFlash = 1.0f - ease * 0.7f;
		glColor4f(0.24f, 0.78f, 1.0f, 0.28f * backFlash);
		DrawSphere(quadric, beamRadius * 3.4f, 18, 18);
		glColor4f(0.96f, 0.99f, 1.0f, 0.82f * backFlash);
		DrawSphere(quadric, beamRadius * 1.35f, 16, 16);
	}
	else
	{
		float phaseT = (t - 0.82f) / 0.18f;
		float fade = 1.0f - ((1.0f - cosf(phaseT * PI)) * 0.5f);
		float radius = 0.10f * fade;

		glColor4f(0.18f, 0.55f, 1.0f, 0.30f * fade);
		DrawSphere(quadric, radius * 1.8f, 16, 16);
		glColor4f(0.54f, 0.88f, 1.0f, 0.42f * fade);
		DrawSphere(quadric, radius * 1.05f, 16, 16);
		glColor4f(0.90f, 0.98f, 1.0f, 0.72f * fade);
		DrawSphere(quadric, radius * 0.58f, 16, 16);
		DrawEnergyHaloRing(radius * 1.9f, radius * 0.55f, 0.18f * fade, 70.0f, swirlAngle, 0.0f);
	}

	glPopMatrix();
	glPopAttrib();
}

void DrawSwordTrailArc(float radius, float thickness, float startDeg, float endDeg, float alpha)
{
	const int segments = 32;

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= segments; ++i)
	{
		float t = (float)i / (float)segments;
		float ang = DegreeToRadian(startDeg + (endDeg - startDeg) * t);
		float cs = cosf(ang);
		float sn = sinf(ang);

		glColor4f(0.10f, 0.55f, 1.0f, 0.0f);
		glVertex3f(cs * (radius + thickness), sn * (radius + thickness), 0.0f);

		glColor4f(0.70f, 0.95f, 1.0f, alpha);
		glVertex3f(cs * radius, sn * radius, 0.0f);
	}
	glEnd();
}

void DrawSwordImpactBurst(float radius, float alpha)
{
	for (int i = 0; i < 8; ++i)
	{
		float angle = DegreeToRadian((float)i * 45.0f);
		float x = cosf(angle);
		float y = sinf(angle);

		glColor4f(0.65f, 0.95f, 1.0f, alpha);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(x * radius, y * radius, 0.0f);
		glEnd();
	}

	glColor4f(0.85f, 0.98f, 1.0f, alpha * 0.85f);
	DrawSphere(quadric, radius * 0.22f, 14, 14);
}

void DrawDualSwordAnimationEffect()
{
	if (!isPlaying || currentSceneMode != ANIMATION || currentAnimType != 9) return;

	float maxFrames = 96.0f;
	float t = Clamp(animFrame / maxFrames, 0.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glPushMatrix();
	glTranslatef(characterX, characterY + 0.12f, characterZ + 0.10f);

	// Rush aura under the body
	if (t > 0.12f && t < 0.82f)
	{
		float rushT = (t - 0.12f) / 0.70f;
		float pulse = 0.55f + 0.45f * sinf(rushT * PI * 10.0f);
		float spread = 0.18f + rushT * 0.24f;

		glColor4f(0.10f, 0.45f, 1.0f, 0.12f + pulse * 0.06f);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-spread, -0.15f, -0.10f);
		glVertex3f(-spread * 0.55f, -0.03f, 0.04f);
		glVertex3f(0.0f, -0.17f, -0.14f);
		glVertex3f(0.0f, -0.01f, 0.10f);
		glVertex3f(spread, -0.15f, -0.10f);
		glVertex3f(spread * 0.55f, -0.03f, 0.04f);
		glEnd();

		for (int i = 0; i < 5; ++i)
		{
			float lane = -0.16f + i * 0.08f;
			glColor4f(0.20f, 0.78f, 1.0f, 0.10f + pulse * 0.05f);
			glBegin(GL_LINES);
			glVertex3f(lane, -0.10f + fabsf(lane) * 0.08f, -0.08f);
			glVertex3f(lane * 0.35f, 0.12f, 0.28f);
			glEnd();
		}
	}

	// Right-hand opening slash: layered crescent with a sharp core
	if (t > 0.24f && t < 0.48f)
	{
		float phaseT = (t - 0.24f) / 0.24f;
		float alpha = 0.50f + 0.30f * sinf(phaseT * PI);
		float sweep = -48.0f + 135.0f * phaseT;

		glPushMatrix();
		glTranslatef(0.13f, 0.03f, 0.03f);
		glRotatef(-26.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(sweep, 0.0f, 0.0f, 1.0f);
		DrawSwordTrailArc(0.20f, 0.14f, -78.0f, 32.0f, alpha * 0.55f);
		DrawSwordTrailArc(0.17f, 0.08f, -72.0f, 26.0f, alpha);

		glColor4f(0.88f, 0.98f, 1.0f, alpha * 0.90f);
		glBegin(GL_LINES);
		glVertex3f(-0.02f, -0.02f, 0.0f);
		glVertex3f(0.24f, 0.10f, 0.0f);
		glEnd();
		glPopMatrix();

		if (phaseT > 0.68f)
		{
			float burstFade = (1.0f - phaseT) * 1.15f;
			glPushMatrix();
			glTranslatef(0.24f, -0.04f, 0.09f);
			glRotatef(22.0f, 0.0f, 1.0f, 0.0f);
			DrawSwordImpactBurst(0.10f + phaseT * 0.10f, burstFade);

			glColor4f(0.30f, 0.80f, 1.0f, burstFade * 0.45f);
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0.0f, 0.0f, 0.0f);
			for (int i = 0; i <= 20; ++i)
			{
				float a = ((float)i / 20.0f) * PI * 2.0f;
				glVertex3f(cosf(a) * (0.14f + phaseT * 0.05f), sinf(a) * (0.05f + phaseT * 0.03f), 0.0f);
			}
			glEnd();
			glPopMatrix();
		}
	}

	// Left-hand follow-through: wider reverse crescent that crosses the body
	if (t > 0.50f && t < 0.76f)
	{
		float phaseT = (t - 0.50f) / 0.26f;
		float alpha = 0.56f + 0.24f * sinf(phaseT * PI);
		float sweep = 60.0f - 162.0f * phaseT;

		glPushMatrix();
		glTranslatef(-0.11f, 0.05f, 0.01f);
		glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(sweep, 0.0f, 0.0f, 1.0f);
		DrawSwordTrailArc(0.24f, 0.15f, -28.0f, 104.0f, alpha * 0.50f);
		DrawSwordTrailArc(0.20f, 0.09f, -18.0f, 94.0f, alpha);

		glColor4f(0.90f, 0.99f, 1.0f, alpha * 0.90f);
		glBegin(GL_LINES);
		glVertex3f(0.02f, -0.01f, 0.0f);
		glVertex3f(-0.26f, 0.11f, 0.0f);
		glEnd();
		glPopMatrix();

		if (phaseT > 0.66f)
		{
			float burstFade = (1.0f - phaseT) * 1.10f;
			glPushMatrix();
			glTranslatef(-0.20f, -0.02f, 0.11f);
			glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
			DrawSwordImpactBurst(0.11f + phaseT * 0.11f, burstFade);

			glColor4f(0.35f, 0.86f, 1.0f, burstFade * 0.38f);
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0.0f, 0.0f, 0.0f);
			for (int i = 0; i <= 20; ++i)
			{
				float a = ((float)i / 20.0f) * PI * 2.0f;
				glVertex3f(cosf(a) * (0.15f + phaseT * 0.05f), sinf(a) * (0.06f + phaseT * 0.02f), 0.0f);
			}
			glEnd();
			glPopMatrix();
		}
	}

	// Crossing flash between the two slashes
	if (t > 0.44f && t < 0.58f)
	{
		float phaseT = (t - 0.44f) / 0.14f;
		float alpha = sinf(phaseT * PI) * 0.65f;

		glPushMatrix();
		glTranslatef(0.01f, 0.04f, 0.08f);
		glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
		glColor4f(0.82f, 0.98f, 1.0f, alpha);
		glBegin(GL_QUADS);
		glVertex3f(-0.22f, -0.02f, 0.0f);
		glVertex3f(-0.18f, 0.02f, 0.0f);
		glVertex3f(0.22f, 0.02f, 0.0f);
		glVertex3f(0.18f, -0.02f, 0.0f);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.00f, 0.03f, 0.07f);
		glRotatef(38.0f, 0.0f, 0.0f, 1.0f);
		glColor4f(0.44f, 0.86f, 1.0f, alpha * 0.65f);
		glBegin(GL_QUADS);
		glVertex3f(-0.20f, -0.018f, 0.0f);
		glVertex3f(-0.16f, 0.018f, 0.0f);
		glVertex3f(0.20f, 0.018f, 0.0f);
		glVertex3f(0.16f, -0.018f, 0.0f);
		glEnd();
		glPopMatrix();
	}

	glPopMatrix();
	glPopAttrib();
}


// ----------------------------------------------------

// ==============
// MODEL ASSEMBLY
// ==============

void DrawCharacter()
{
	bool isChibi = (currentCharacterMode == CHIBI_MODE);

	// handsome / tall mode proportions
	float torsoRadius = isChibi ? 0.052f : 0.06f;
	float torsoHeight = isChibi ? 0.042f : 0.03f;

	float neckRadius = isChibi ? 0.017f : 0.02f;
	float neckHeight = isChibi ? 0.078f : 0.065f;

	float headBaseRadius = isChibi ? 0.082f : 0.10f;
	float headBaseHeight = isChibi ? 0.040f : 0.05f;

	float torsoArmsOffsetX = isChibi ? 0.090f : 0.095f;
	float torsoLegOffsetX = 0.04f;

	glPushMatrix();

	// =========================
	// UPPER TORSO
	// =========================
	glPushMatrix();
	PartRotation& upperTorso = parts[UPPER_TORSO];
	glRotatef(upperTorso.angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(upperTorso.angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(upperTorso.angleZ, 0.0f, 0.0f, 1.0f);

	DrawTorsoPart(torsoRadius, torsoHeight);

	// Red Vest
	glPushMatrix();
	ApplyTint(GetElementClothTint());
	DrawVest(torsoRadius, torsoHeight);
	ResetTint();
	glPopMatrix();

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

	// =========================
	// NECK
	// =========================
	glPushMatrix();
	glTranslatef(0.0f, (torsoHeight / 2 + neckHeight * (isChibi ? 0.82f : 1.1f)), 0.0f);
	DrawNeck(neckRadius, neckHeight);

	// =========================
	// HEAD
	// =========================
	glPushMatrix();
	glTranslatef(0.0f, neckHeight + headBaseHeight * (isChibi ? 0.95f : 0.8f), 0.0f);

	PartRotation& head = parts[HEAD];
	glRotatef(head.angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(head.angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(head.angleZ, 0.0f, 0.0f, 1.0f);
	DrawHead(headBaseRadius, headBaseHeight);

	// Hair
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * (isChibi ? 0.46f : 0.5f), 0.0f);
	ApplyTint(GetElementHairTint());

	if (isChibi)
	{
		glScalef(0.92f, 0.96f, 0.92f);
	}
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
	glTranslatef(0.0f, headBaseRadius * (isChibi ? 0.92f : 1.0f), 0.0f);
	ApplyTint(GetElementClothTint());
	if (isChibi)
		glScalef(0.90f, 0.92f, 0.90f);
	DrawHairRibbons(headBaseRadius);
	ResetTint();
	glPopMatrix();

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

	// =========================
	// ARMS
	// =========================
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glTranslatef(0.0f, torsoHeight * (isChibi ? 0.92f : 1.0f), 0.0f);

	// Left Arm
	glPushMatrix();
	glTranslatef(-torsoArmsOffsetX, 0.0f, 0.0f);
	if (isChibi)
		glScalef(0.92f, 1.20f, 0.92f);
	DrawArm(-1.0f);
	glPopMatrix();

	// Right Arm
	glPushMatrix();
	glTranslatef(torsoArmsOffsetX, 0.0f, 0.0f);
	if (isChibi)
		glScalef(0.92f, 1.20f, 0.92f);
	DrawArm(1.0f);
	glPopMatrix();

	glPopMatrix();
	// END Arms

	glPopMatrix();
	// END Upper Torso

	glDisable(GL_TEXTURE_2D);

	// =========================
	// LOWER TORSO
	// =========================
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight, 0.0f);
	PartRotation& lowerTorso = parts[LOWER_TORSO];
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

	// Upper Pant
	float upperPantRadius = 0.034f;
	float upperPantHeight = torsoRadius * 2.5f;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brownClothTexture);

	// Waistband
	glPushMatrix();
	glTranslatef(0.0f, -0.03f * 2.5f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, upperPantRadius, upperPantRadius, upperPantHeight, SLICES, STACKS);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	// =========================
	// LEGS
	// =========================
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skinTexture);

	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 2.5f, 0.0f);

	// Left Leg
	glPushMatrix();
	glTranslatef(-torsoLegOffsetX, 0.0f, 0.0f);
	DrawLeg(-1.0f, torsoRadius);
	glPopMatrix();

	// Right Leg
	glPushMatrix();
	glTranslatef(torsoLegOffsetX, 0.0f, 0.0f);
	DrawLeg(1.0f, torsoRadius);
	glPopMatrix();

	glPopMatrix();
	// END Legs

	glPopMatrix();
	// END Lower Torso

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

// ------------------
// ENVIRONMENT SETUP
// ------------------

void DrawSky(float radius)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glPushMatrix();

	// Optional: keep the sky fixed relative to camera
	glTranslatef(cameraX, cameraY, cameraZ);

	// Invert normals for inside view
	gluQuadricOrientation(quadric, GLU_INSIDE);
	//glColor3f(0.53f, 0.81f, 0.92f); // light blue sky color

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawSphere(quadric, radius, SLICES, STACKS);
	glPopMatrix();

	gluQuadricOrientation(quadric, GLU_OUTSIDE); // restore
	glDisable(GL_TEXTURE_2D);
}

void DrawGround(float width, float depth)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, groundTexture);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ground_matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ground_matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ground_matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, ground_shininess);

	glPushMatrix();
	DrawPlane(width, depth);
	glPopMatrix();

	ResetMaterial();

	glDisable(GL_TEXTURE_2D);
}

void DrawWorld(float radius)
{
	// Sky
	glPushMatrix();
	glDisable(GL_LIGHTING);
	DrawSky(radius);
	glEnable(GL_LIGHTING);

	// Ground
	glPushMatrix();
	DrawGround(radius, radius);
	glPopMatrix();
	// END Ground

	glPopMatrix();
	// END Sky
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

	DrawFireElementBackgroundEffect();
	DrawWaterElementBackgroundEffect();
	DrawWoodElementBackgroundEffect();
	DrawMetalElementBackgroundEffect();
	DrawEarthElementBackgroundEffect();


	float worldRadius = 100.0f;
	float worldOffsetY = -0.55f;

	// World
	glPushMatrix();
	glTranslatef(0.0f, worldOffsetY, 0.0f);
	DrawWorld(worldRadius);
	glPopMatrix();
	// END World

	// ZhaLing
	glPushMatrix();
	glTranslatef(characterX, characterY, characterZ);
	DrawCharacter();
	glPopMatrix();
	// END ZhaLing

	DrawEnergyBeamAnimationEffect();
	DrawDualSwordAnimationEffect();

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
