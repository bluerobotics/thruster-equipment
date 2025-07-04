#pragma once

#include <cstdint>
#include "ClearCore.h"
#include "EthernetUdp.h"
#include "IpAddress.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// --- System Parameters & Conversions ---
#define PITCH_MM_PER_REV 5.0f
#define pulsesPerRev 800
#define STEPS_PER_MM_M0 160.0f
#define STEPS_PER_MM_M1 160.0f
#define STEPS_PER_MM_M2 160.0f
#define MAX_HOMING_DURATION_MS 30000 // 30 seconds

// --- Command Strings & Prefixes ---
#define CMD_STR_REQUEST_TELEM "REQUEST_TELEM"
#define CMD_STR_DISCOVER "DISCOVER_INJECTOR"
#define CMD_STR_SET_PEER_IP "SET_PEER_IP "
#define CMD_STR_CLEAR_PEER_IP "CLEAR_PEER_IP"
#define CMD_STR_ENABLE "ENABLE"
#define CMD_STR_DISABLE "DISABLE"
#define CMD_STR_ABORT "ABORT"
#define CMD_STR_CLEAR_ERRORS "CLEAR_ERRORS"
#define CMD_STR_STANDBY_MODE "STANDBY_MODE"
#define CMD_STR_JOG_MODE "JOG_MODE"
#define CMD_STR_HOMING_MODE "HOMING_MODE"
#define CMD_STR_FEED_MODE "FEED_MODE"
#define CMD_STR_SET_INJECTOR_TORQUE_OFFSET "SET_INJECTOR_TORQUE_OFFSET "
#define CMD_STR_SET_PINCH_TORQUE_OFFSET "SET_PINCH_TORQUE_OFFSET "
#define CMD_STR_JOG_MOVE "JOG_MOVE "
#define CMD_STR_MACHINE_HOME_MOVE "MACHINE_HOME_MOVE "
#define CMD_STR_CARTRIDGE_HOME_MOVE "CARTRIDGE_HOME_MOVE "
#define CMD_STR_PINCH_HOME_MOVE "PINCH_HOME_MOVE"
#define CMD_STR_INJECT_MOVE "INJECT_MOVE "
#define CMD_STR_PURGE_MOVE "PURGE_MOVE "
#define CMD_STR_PINCH_OPEN "PINCH_OPEN"
#define CMD_STR_PINCH_CLOSE "PINCH_CLOSE"
#define CMD_STR_PINCH_JOG_MOVE "PINCH_JOG_MOVE "
#define CMD_STR_ENABLE_PINCH "ENABLE_PINCH"
#define CMD_STR_DISABLE_PINCH "DISABLE_PINCH"
#define CMD_STR_MOVE_TO_CARTRIDGE_HOME "MOVE_TO_CARTRIDGE_HOME"
#define CMD_STR_MOVE_TO_CARTRIDGE_RETRACT "MOVE_TO_CARTRIDGE_RETRACT "
#define CMD_STR_PAUSE_INJECTION "PAUSE_INJECTION"
#define CMD_STR_RESUME_INJECTION "RESUME_INJECTION"
#define CMD_STR_CANCEL_INJECTION "CANCEL_INJECTION"

#define TELEM_PREFIX_GUI "INJ_TELEM_GUI:"
#define STATUS_PREFIX_INFO "INJ_INFO: "
#define STATUS_PREFIX_START "INJ_START: "
#define STATUS_PREFIX_DONE "INJ_DONE: "
#define STATUS_PREFIX_ERROR "INJ_ERROR: "
#define STATUS_PREFIX_DISCOVERY "DISCOVERY: "

#define EWMA_ALPHA 0.2f
#define TORQUE_SENTINEL_INVALID_VALUE -9999.0f
#define MAX_PACKET_LENGTH 512
#define LOCAL_PORT 8888

// --- State Enums ---
enum MainState : uint8_t { STANDBY_MODE, JOG_MODE, HOMING_MODE, FEED_MODE, DISABLED_MODE, MAIN_STATE_COUNT };
enum HomingState : uint8_t { HOMING_NONE, HOMING_MACHINE, HOMING_CARTRIDGE, HOMING_PINCH, HOMING_STATE_COUNT };
enum HomingPhase : uint8_t { HOMING_PHASE_IDLE, HOMING_PHASE_STARTING_MOVE, HOMING_PHASE_RAPID_MOVE, HOMING_PHASE_BACK_OFF, HOMING_PHASE_TOUCH_OFF, HOMING_PHASE_RETRACT, HOMING_PHASE_COMPLETE, HOMING_PHASE_ERROR, HOMING_PHASE_COUNT };
enum FeedState : uint8_t { FEED_NONE, FEED_STANDBY, FEED_INJECT_STARTING, FEED_INJECT_ACTIVE, FEED_INJECT_PAUSED, FEED_INJECT_RESUMING, FEED_PURGE_STARTING, FEED_PURGE_ACTIVE, FEED_PURGE_PAUSED, FEED_PURGE_RESUMING, FEED_MOVING_TO_HOME, FEED_MOVING_TO_RETRACT, FEED_INJECTION_CANCELLED, FEED_INJECTION_COMPLETED, FEED_STATE_COUNT };
enum ErrorState : uint8_t { ERROR_NONE, ERROR_MANUAL_ABORT, ERROR_TORQUE_ABORT, ERROR_MOTION_EXCEEDED_ABORT, ERROR_NO_CARTRIDGE_HOME, ERROR_NO_MACHINE_HOME, ERROR_HOMING_TIMEOUT, ERROR_HOMING_NO_TORQUE_RAPID, ERROR_HOMING_NO_TORQUE_TOUCH, ERROR_INVALID_INJECTION, ERROR_NOT_HOMED, ERROR_INVALID_PARAMETERS, ERROR_MOTORS_DISABLED, ERROR_STATE_COUNT };
typedef enum {
	CMD_UNKNOWN,
	CMD_STANDBY,
	CMD_JOG,
	CMD_HOMING,
	CMD_FEED,
	CMD_ENABLE,
	CMD_DISABLE,
	CMD_ENABLE_PINCH,
	CMD_DISABLE_PINCH,
	CMD_JOG_MOVE,
	CMD_PINCH_JOG_MOVE,
	CMD_MACHINE_HOME_MOVE,
	CMD_CARTRIDGE_HOME_MOVE,
	CMD_PINCH_HOME_MOVE,
	CMD_MOVE_TO_CARTRIDGE_HOME,
	CMD_MOVE_TO_CARTRIDGE_RETRACT,
	CMD_INJECT_MOVE,
	CMD_PURGE_MOVE,
	CMD_PINCH_OPEN,
	CMD_PINCH_CLOSE,
	CMD_PAUSE_INJECTION,
	CMD_RESUME_INJECTION,
	CMD_CANCEL_INJECTION,
	CMD_SET_TORQUE_OFFSET,
	CMD_DISCOVER,
	CMD_REQUEST_TELEM,
	CMD_ABORT,
	CMD_CLEAR_ERRORS,
	CMD_SET_PEER_IP,
	CMD_CLEAR_PEER_IP,
	CMD_STANDBY_MODE,
	CMD_JOG_MODE,
	CMD_HOMING_MODE,
	CMD_FEED_MODE
} UserCommand;

class Injector {
	public:
	MainState mainState;
	HomingState homingState;
	HomingPhase currentHomingPhase;
	FeedState feedState;
	ErrorState errorState;

	EthernetUdp udp;
	IpAddress guiIp;
	uint16_t guiPort;
	bool guiDiscovered;
	IpAddress peerIp;
	bool peerDiscovered;

	bool homingMachineDone;
	bool homingCartridgeDone;
	bool homingPinchDone;
	bool feedingDone;
	bool jogDone;
	uint32_t homingStartTime;
	bool motorsAreEnabled;
	
	float homing_stroke_mm_param;
	float homing_rapid_vel_mm_s_param;
	float homing_touch_vel_mm_s_param;
	float homing_acceleration_param;
	float homing_retract_mm_param;
	float homing_torque_percent_param;
	long homing_actual_stroke_steps;
	long homing_actual_retract_steps;
	int homing_actual_rapid_sps;
	int homing_actual_touch_sps;
	int homing_actual_accel_sps2;
	long homingDefaultBackoffSteps;
	
	int feedDefaultTorquePercent;
	long feedDefaultVelocitySPS;
	long feedDefaultAccelSPS2;
	
	// Constructor
	Injector();

	// Core Functions
	void setup();
	void loop();
	void updateState();

	// Communication
	void sendGuiTelemetry(void);
	void setupSerial();
	void setupEthernet();
	void processUdp();
	void parseUserCommand(const char *msg);
	void sendStatus(const char* statusType, const char* message);
	UserCommand parseCommand(const char* msg);
	
	//--- Motor Functions ---//
	void setupMotors(void);
	void enableInjectorMotors(const char* reason_message);
	void disableInjectorMotors(const char* reason_message);
	void moveInjectorMotors(int stepsM0, int stepsM1, int torque_limit, int velocity, int accel);
	void movePinchMotor(int stepsM2, int torque_limit, int velocity, int accel);
	bool checkInjectorMoving(void);
	float getSmoothedTorqueEWMA(MotorDriver *motor, float *smoothedValue, bool *firstRead);
	bool checkInjectorTorqueLimit(void);
	void abortInjectorMove(void);

	// Command Handlers
	void handleMessage(const char* msg);
	void handleEnable();
	void handleDisable();
	void handleAbort();
	void handleClearErrors();
	void handleStandbyMode();
	void handleJogMode();
	void handleHomingMode();
	void handleFeedMode();
	void handleSetinjectorMotorsTorqueOffset(const char* msg);
	void handleJogMove(const char* msg);
	void handleMachineHomeMove(const char* msg);
	void handleCartridgeHomeMove(const char* msg);
	void handlePinchHomeMove();
	void handlePinchJogMove(const char* msg);
	void handleEnablePinch();
	void handleDisablePinch();
	void handleMoveToCartridgeHome();
	void handleMoveToCartridgeRetract(const char* msg);
	void handleInjectMove(const char* msg);
	void handlePurgeMove(const char* msg);
	void handlePauseOperation();
	void handleResumeOperation();
	void handleCancelOperation();
	void handleSetPeerIp(const char* msg);
	void handleClearPeerIp();
	void handlePinchOpen();
	void handlePinchClose();

	//--- State Trigger Functions ---//
	void onHomingMachineDone(void);
	void onHomingCartridgeDone(void);
	void onHomingPinchDone(void);
	void onFeedingDone(void);
	void onJogDone(void);
	
	//--- Reset Functions ---//
	void resetMotors(void);
	void finalizeAndResetActiveDispenseOperation(bool operationCompletedSuccessfully);
	void fullyResetActiveDispenseOperation(void);
	void resetActiveDispenseOp(void);

	private:
	const char* mainStateStr() const;
	const char* homingStateStr() const;
	const char* homingPhaseStr() const;
	const char* feedStateStr() const;
	const char* errorStateStr() const;

	uint32_t lastGuiTelemetryTime;

	char telemetryBuffer[512];
	unsigned char packetBuffer[MAX_PACKET_LENGTH];

	// Other private member variables...
	float injectorMotorsTorqueLimit;
	float injectorMotorsTorqueOffset;
	float smoothedTorqueValue0, smoothedTorqueValue1, smoothedTorqueValue2;
	bool firstTorqueReading0, firstTorqueReading1, firstTorqueReading2;
	int32_t machineHomeReferenceSteps, cartridgeHomeReferenceSteps;

	// Dispense operation variables
	float active_op_target_ml, active_op_total_dispensed_ml, active_op_steps_per_ml, last_completed_dispense_ml;
	long active_op_total_target_steps, active_op_remaining_steps, active_op_segment_initial_axis_steps, active_op_initial_axis_steps;
	bool active_dispense_INJECTION_ongoing;
	int active_op_velocity_sps, active_op_accel_sps2, active_op_torque_percent;

	static const char *MainStateNames[MAIN_STATE_COUNT];
	static const char *HomingStateNames[HOMING_STATE_COUNT];
	static const char *HomingPhaseNames[HOMING_PHASE_COUNT];
	static const char *FeedStateNames[FEED_STATE_COUNT];
	static const char *ErrorStateNames[ERROR_STATE_COUNT];
};
