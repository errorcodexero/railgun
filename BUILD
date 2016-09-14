cc_binary(
	name = "point_test",
	srcs = ["util/point.cpp","util/point.h","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"],
	copts = ["-DPOINT_TEST"]
)

cc_binary(
	name = "type_test",
	srcs = ["util/type.cpp","util/type.h"],
	copts = ["-DTYPE_TEST"]
)

cc_library(
	name = "point",
	srcs = ["util/point.cpp"],
	hdrs = ["util/point.h","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"]
)

cc_binary(
	name="util_test",
	#srcs=["util/util.cpp","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"],
	srcs=["util/util.cpp"],
	copts=["-DUTIL_TEST"],
	deps=[":point"]
)

cc_library(
	name="util",
	srcs=["util/util.cpp"],
	deps=[":point"]
)

cc_binary(
	name="jag_interface_test",
	srcs=["util/jag_interface.cpp"],
	copts=["-DJAG_INTERFACE_TEST"],
	deps=[":util"]
)

cc_library(name="jag_interface",srcs=["util/jag_interface.cpp"],deps=[":util"])

cc_binary(
	name="driver_station_interface_test",
	srcs=["util/driver_station_interface.cpp"],
	copts=["-DDRIVER_STATION_INTERFACE_TEST"],
	deps=[":util"]
)

cc_library(
	name="driver_station_interface",
	srcs=["util/driver_station_interface.cpp"],
	deps=[":util"]
)

cc_binary(
	name="interface_test",
	srcs=["util/interface.cpp"],
	copts=["-DINTERFACE_TEST"],
	deps=[":jag_interface",":driver_station_interface"]
)

cc_library(
	name="posedge_toggle",
	srcs=["util/posedge_toggle.cpp"],
	hdrs=["util/posedge_toggle.h"],
	deps=[":jag_interface"]
)


cc_library(
	name="type",
	srcs=["util/type.cpp"],
	hdrs=["util/type.h"]
)

cc_library(
	name="countup_timer",
	srcs=["util/countup_timer.cpp"],
	hdrs=["util/countup_timer.h"],
	deps=[":interface"]
)

cc_binary(
	name="countup_timer_test",
	srcs=["util/countup_timer.cpp","util/countup_timer.h"],
	copts=["-DCOUNTUP_TIMER_TEST"],
	deps=[":interface"]
)

cc_library(
	name="countdown_timer",
	srcs=["util/countdown_timer.cpp"],
	hdrs=["util/countdown_timer.h"],
	deps=[":util"]
)

cc_binary(
	name="countdown_timer_test",
	srcs=["util/countdown_timer.cpp","util/countdown_timer.h"],
	copts=["-DCOUNTDOWN_TIMER_TEST"],
	deps=[":util"]
)

cc_library(
	name="network",
	srcs=["util/network.cpp"],
	hdrs=["util/network.h"]
)

cc_binary(
	name="network_test",
	srcs=["util/network.cpp","util/network.h"],
	copts=["-DNETWORK_TEST"],
	deps=[]
)

cc_library(
	name="negedge_trigger",
	srcs=["util/negedge_trigger.cpp"],
	hdrs=["util/negedge_trigger.h"]
)

cc_binary(
	name="negedge_trigger_test",
	srcs=["util/negedge_trigger.cpp","util/negedge_trigger.h"],
	copts=["-DNEGEDGE_TRIGGER_TEST"],
	deps=[]
)

cc_library(
	name="nav2",
	srcs=["util/nav2.cpp"],
	hdrs=["util/nav2.h"],
	deps=[":nav"]
)

cc_binary(
	name="nav2_test",
	srcs=["util/nav2.cpp","util/nav2.h"],
	copts=["-DNAV2_TEST"],
	deps=[":nav"]
)

cc_library(
	name="string_utils",
	srcs=["util/string_utils.cpp"],
	hdrs=["util/string_utils.h"]
)

cc_binary(
	name="string_utils_test",
	srcs=["util/string_utils.cpp","util/string_utils.h"],
	copts=["-DSTRING_UTILS_TEST"],
	deps=[]
)

cc_library(
	name="posedge_trigger",
	srcs=["util/posedge_trigger.cpp"],
	hdrs=["util/posedge_trigger.h"]
)

cc_binary(
	name="posedge_trigger_test",
	srcs=["util/posedge_trigger.cpp","util/posedge_trigger.h"],
	copts=["-DPOSEDGE_TRIGGER_TEST"],
	deps=[]
)

cc_library(
	name="debounce",
	srcs=["util/debounce.cpp"],
	hdrs=["util/debounce.h"],
	deps=[":countdown_timer"]
)

cc_binary(
	name="debounce_test",
	srcs=["util/debounce.cpp","util/debounce.h"],
	copts=["-DDEBOUNCE_TEST"],
	deps=[":countdown_timer"]
)

cc_library(
	name="pwm",
	srcs=["util/pwm.cpp"],
	hdrs=["util/pwm.h"],
)

cc_library(
	name="interface",
	srcs=["util/interface.cpp"],
	hdrs=["util/interface.h"],
	deps=[":jag_interface",":driver_station_interface",":pwm"]
)

cc_library(
	name="settable_toggle",
	srcs=["util/settable_toggle.cpp"],
	hdrs=["util/settable_toggle.h"],
	deps=[":posedge_trigger"]
)

cc_binary(
	name="settable_toggle_test",
	srcs=["util/settable_toggle.cpp","util/settable_toggle.h"],
	copts=["-DSETTABLE_TOGGLE_TEST"],
	deps=[":posedge_trigger"]
)

cc_library(
	name="bound_tracker",
	srcs=["util/bound_tracker.cpp"],
	hdrs=["util/bound_tracker.h"]
)

cc_binary(
	name="bound_tracker_test",
	srcs=["util/bound_tracker.cpp","util/bound_tracker.h"],
	copts=["-DBOUND_TRACKER_TEST"],
	deps=[]
)

cc_library(
	name="fixVictor",
	srcs=["util/fixVictor.cpp"],
	hdrs=["util/fixVictor.h"]
)

cc_binary(
	name="fixVictor_test",
	srcs=["util/fixVictor.cpp","util/fixVictor.h"],
	copts=["-DFIXVICTOR_TEST"],
	deps=[]
)

cc_library(
	name="input",
	srcs=["util/input.cpp"],
	hdrs=["util/input.h"],
	deps=[":interface"]
)

cc_binary(
	name="input_test",
	srcs=["util/input.cpp","util/input.h"],
	copts=["-DINPUT_TEST"],
	deps=[":interface"]
)

cc_library(
	name="nav",
	srcs=["util/nav.cpp"],
	hdrs=["util/nav.h"]
)

cc_binary(
	name="nav_test",
	srcs=["util/nav.cpp","util/nav.h"],
	copts=["-DNAV_TEST"],
	deps=[]
)

cc_library(
	name="output",
	srcs=["util/output.cpp"],
	hdrs=["util/output.h"],
	deps=[":interface"]
)

cc_binary(
	name="output_test",
	srcs=["util/output.cpp","util/output.h"],
	copts=["-DOUTPUT_TEST"],
	deps=[":interface"]
)

cc_library(
	name="motion_profile",
	srcs=["util/motion_profile.cpp"],
	hdrs=["util/motion_profile.h"]
)

cc_binary(
	name="motion_profile_test",
	srcs=["util/motion_profile.cpp","util/motion_profile.h"],
	copts=["-DMOTION_PROFILE_TEST"],
	deps=[]
)

cc_library(
	name="perf_tracker",
	srcs=["util/perf_tracker.cpp"],
	hdrs=["util/perf_tracker.h"],
	deps=[":interface"]
)

cc_binary(
	name="perf_tracker_test",
	srcs=["util/perf_tracker.cpp","util/perf_tracker.h"],
	copts=["-DPERF_TRACKER_TEST"],
	deps=[":interface"]
)

cc_library(
	name="posedge_trigger_debounce",
	srcs=["util/posedge_trigger_debounce.cpp"],
	hdrs=["util/posedge_trigger_debounce.h"],
	deps=[":debounce",":posedge_trigger"]
)

cc_binary(
	name="posedge_trigger_debounce_test",
	srcs=["util/posedge_trigger_debounce.cpp","util/posedge_trigger_debounce.h"],
	copts=["-DPOSEDGE_TRIGGER_DEBOUNCE_TEST"],
	deps=[":debounce",":posedge_trigger"]
)

cc_library(
	name="meta",
	srcs=["util/meta.cpp"],
	hdrs=["util/meta.h"],
	deps=[":interface",":tilt"]
)

cc_binary(
	name="meta_test",
	srcs=["util/meta.cpp","util/meta.h"],
	copts=["-DMETA_TEST"],
	deps=[":interface",":tilt",":front"]
)

cc_library(
	name="monitor",
	srcs=["control/monitor.cpp"],
	hdrs=["control/monitor.h"],
	deps=[":main"]
)

cc_binary(
	name="monitor_test",
	srcs=["control/monitor.cpp","control/monitor.h"],
	copts=["-DMONITOR_TEST"],
	deps=[":main"]
)

cc_library(
	name="motor_check",
	srcs=["control/motor_check.cpp"],
	hdrs=["control/motor_check.h"],
	deps=[":util"]
)

cc_binary(
	name="motor_check_test",
	srcs=["control/motor_check.cpp","control/motor_check.h"],
	copts=["-DMOTOR_CHECK_TEST"],
	deps=[":util"]
)

cc_library(
	name="main",
	srcs=["control/main.cpp"],
	hdrs=["control/main.h"],
	deps=[":force_interface",":posedge_toggle",":perf_tracker",":countdown_timer",":countup_timer",":toplevel",":panel",":nav",":nav2",":log",":posedge_trigger_debounce",":motion_profile",":tilt_presets",":shooter_constants",":teleop"]
)

cc_binary(
	name="main_test",
	srcs=["control/main.cpp","control/main.h"],
	copts=["-DMAIN_TEST"],
	deps=[":force_interface",":toplevel",":posedge_toggle",":perf_tracker",":countup_timer",":panel",":nav2",":log",":posedge_trigger_debounce",":motion_profile",":tilt_presets",":monitor",":shooter_constants"]
)

cc_library(
	name="log",
	srcs=["control/log.cpp"],
	hdrs=["control/log.h"],
	deps=[":interface",":toplevel"]
)

cc_binary(
	name="log_test",
	srcs=["control/log.cpp","control/log.h","control/formal.h"],
	copts=["-DLOG_TEST"],
	deps=[":interface",":toplevel"]
)

cc_library(
	name="drivebase",
	srcs=["control/drivebase.cpp"],
	hdrs=["control/drivebase.h","util/quick.h"],
	deps=[":interface",":motor_check",":countdown_timer",":fixVictor"]
)

cc_binary(
	name="drivebase_test",
	srcs=["control/drivebase.cpp","control/drivebase.h","util/quick.h","control/formal.h"],
	copts=["-DDRIVEBASE_TEST"],
	deps=[":interface",":motor_check",":countdown_timer",":fixVictor"]
)

cc_library(
	name="climb_release",
	srcs=["control/climb_release.cpp"],
	hdrs=["control/climb_release.h"],
	deps=[":countdown_timer"]
)

cc_binary(
	name="climb_release_test",
	srcs=["control/climb_release.cpp","control/climb_release.h","control/formal.h"],
	copts=["-DCLIMB_RELEASE_TEST"],
	deps=[":countdown_timer",":interface"]
)

cc_library(
	name="force",
	srcs=["control/force.cpp"],
	hdrs=["control/force.h"],
	deps=[":interface"]
)

#cc_binary(
#	name="force_interface_test",
#	srcs=["control/force_interface.cpp","control/force_interface.h"],
#	copts=["-DFORCE_INTERFACE_TEST"],
#	deps=[":posedge_trigger"]
#)

cc_library(
	name="force_interface",
	srcs=["control/force_interface.cpp"],
	hdrs=["control/force_interface.h"],
	deps=[":posedge_trigger",":force"]
)

cc_binary(
	name="force_interface_test",
	srcs=["control/force_interface.cpp","control/force_interface.h"],
	copts=["-DFORCE_INTERFACE_TEST"],
	deps=[":posedge_trigger",":force",":interface"]
)

cc_library(
	name="toplevel",
	srcs=["control/toplevel.cpp"],
	hdrs=["control/toplevel.h"],
	deps=[":pump",":drivebase",":climb_release",":winch",":collector",":shooter",":input"]
)

cc_binary(
	name="toplevel_test",
	srcs=["control/toplevel.cpp","control/toplevel.h","control/formal.h"],
	copts=["-DTOPLEVEL_TEST"],
	deps=[":pump",":drivebase",":climb_release",":winch",":collector",":shooter",":input",":output"]
)

cc_library(
	name="tilt",
	srcs=["control/tilt.cpp"],
	hdrs=["control/tilt.h","util/quick.h"],
	deps=[":util",":countdown_timer"]
)

cc_binary(
	name="tilt_test",
	srcs=["control/tilt.cpp","control/tilt.h","util/quick.h","control/formal.h"],
	copts=["-DTILT_TEST"],
	deps=[":util",":countdown_timer",":interface"]
)

cc_library(
	name="tilt_presets",
	srcs=["control/tilt_presets.cpp"],
	hdrs=["control/tilt_presets.h"],
	deps=[]
)

cc_binary(
	name="tilt_presets_test",
	srcs=["control/tilt_presets.cpp","control/tilt_presets.h"],
	copts=["-DTILT_PRESETS_TEST"],
	deps=[]
)

cc_library(
	name="sim",
	srcs=["control/sim.cpp"],
	deps=[]
)

cc_binary(
	name="sim_test",
	srcs=["control/sim.cpp"],
	copts=["-DSIM_TEST"],
	deps=[":toplevel"]
)

cc_library(
	name="sides",
	srcs=["control/sides.cpp"],
	hdrs=["control/sides.h"],
	deps=[":interface",":nop"]
)

cc_binary(
	name="sides_test",
	srcs=["control/sides.cpp","control/sides.h","control/formal.h"],
	copts=["-DSIDES_TEST"],
	deps=[":interface",":nop"]
)

cc_library(
	name="front",
	srcs=["control/front.cpp"],
	hdrs=["control/front.h"],
	deps=[":interface"]
)

cc_binary(
	name="front_test",
	srcs=["control/front.cpp","control/front.h","control/formal.h"],
	copts=["-DFRONT_TEST"],
	deps=[":interface"]
)

cc_library(
	name="pump",
	srcs=["control/pump.cpp"],
	hdrs=["control/pump.h","util/maybe.h"],
	deps=[":interface"]
)

cc_binary(
	name="pump_test",
	srcs=["control/pump.cpp","control/pump.h","util/maybe.h","control/formal.h"],
	copts=["-DPUMP_TEST"],
	deps=[":interface"]
)

cc_library(
	name="tote_sensors",
	srcs=["control/tote_sensors.cpp"],
	hdrs=["control/tote_sensors.h"],
	deps=[":nop"]
)

cc_binary(
	name="tote_sensors_test",
	srcs=["control/tote_sensors.cpp","control/tote_sensors.h","control/formal.h"],
	copts=["-DTOTE_SENSORS_TEST"],
	deps=[":nop"]
)

cc_library(
	name="nop",
	srcs=["control/nop.cpp"],
	hdrs=["control/nop.h"],
	deps=[":interface"]
)

cc_binary(
	name="nop_test",
	srcs=["control/nop.cpp","control/nop.h","control/formal.h"],
	copts=["-DNOP_TEST"],
	deps=[":interface"]
)

cc_library(
	name="collector",
	srcs=["control/collector.cpp"],
	hdrs=["control/collector.h"],
	deps=[":front",":sides",":tilt"]
)

cc_binary(
	name="collector_test",
	srcs=["control/collector.cpp","control/collector.h","control/formal.h"],
	copts=["-DCOLLECTOR_TEST"],
	deps=[":front",":sides",":tilt"]
)

cc_library(
	name="shooter",
	srcs=["control/shooter.cpp"],
	hdrs=["control/shooter.h"],
	deps=[":interface",":countdown_timer"]
)

cc_binary(
	name="shooter_test",
	srcs=["control/shooter.cpp","control/shooter.h","control/formal.h"],
	copts=["-DSHOOTER_TEST"],
	deps=[":countdown_timer",":interface"]
)

cc_library(
	name="winch",
	srcs=["control/winch.cpp"],
	hdrs=["control/winch.h"],
	deps=[":interface",":countdown_timer"]
)

cc_binary(
	name="winch_test",
	srcs=["control/winch.cpp","control/winch.h","control/formal.h"],
	copts=["-DWINCH_TEST"],
	deps=[":interface",":countdown_timer"]
)

cc_library(
	name="input_util",
	srcs=["input/util.cpp"],
	hdrs=["input/util.h"],
	deps=[":interface"]
)

cc_library(
	name="panel",
	srcs=["input/panel.cpp"],
	hdrs=["input/panel.h","util/maybe.h"],
	deps=[":interface",":input_util"]
)

cc_library(
	name="portcullis",
	srcs=["modes/portcullis.cpp"],
	hdrs=["modes/portcullis.h"],
	deps=[":mode",":teleop",":tilt_presets"]
)

cc_binary(
	name="portcullis_test",
	srcs=["modes/portcullis.cpp","modes/portcullis.h"],
	copts=["-DPORTCULLIS_TEST"],
	deps=[":mode",":teleop",":tilt_presets"]
)

cc_library(
	name="auto_reach",
	srcs=["modes/auto_reach.cpp"],
	hdrs=["modes/auto_reach.h"],
	deps=[":mode",":teleop",":auto_stop"]
)

cc_binary(
	name="auto_reach_test",
	srcs=["modes/auto_reach.cpp","modes/auto_reach.h"],
	copts=["-DAUTO_REACH_TEST"],
	deps=[":mode",":teleop",":auto_reach"]
)

cc_library(
	name="cheval",
	srcs=["modes/cheval.cpp"],
	hdrs=["modes/cheval.h"],
	deps=[":mode",":teleop"]
)

cc_binary(
	name="cheval_test",
	srcs=["modes/cheval.cpp","modes/cheval.h"],
	copts=["-DCHEVAL_TEST"],
	deps=[":mode",":teleop"]
)

cc_library(
	name="low_bar_wall_high_score",
	srcs=["modes/low_bar_wall_high_score.cpp"],
	hdrs=["modes/low_bar_wall_high_score.h"],
	deps=[":mode"]
)

cc_binary(
	name="low_bar_wall_high_score_test",
	srcs=["modes/low_bar_wall_high_score.cpp","modes/low_bar_wall_high_score.h"],
	copts=["-DLOW_BAR_WALL_HIGH_SCORE_TEST"],
	deps=[":mode"]
)

cc_library(
	name="teleop",
	srcs=["modes/teleop.cpp"],
	hdrs=["modes/teleop.h"],
	deps=[":mode",":posedge_trigger_debounce",":posedge_toggle",":tilt_presets",":shooter_constants"]
)

cc_binary(
	name="teleop_test",
	srcs=["modes/teleop.cpp","modes/teleop.h"],
	copts=["-DTELEOP_TEST"],
	deps=[":mode",":posedge_trigger_debounce",":posedge_toggle",":tilt_presets",":shooter_constants"]
)

cc_library(
	name="low_bar_wall_low_score",
	srcs=["modes/low_bar_wall_low_score.cpp"],
	hdrs=["modes/low_bar_wall_low_score.h"],
	deps=[":mode"]
)

cc_binary(
	name="low_bar_wall_low_score_test",
	srcs=["modes/low_bar_wall_low_score.cpp","modes/low_bar_wall_low_score.h"],
	copts=["-DLOW_BAR_WALL_LOW_SCORE_TEST"],
	deps=[":mode"]
)

cc_library(
	name="barrelracer",
	srcs=["modes/barrelracer.cpp"],
	hdrs=["modes/barrelracer.h"],
	deps=[":mode",":teleop",":motion_profile"]
)

cc_binary(
	name="barrelracer_test",
	srcs=["modes/barrelracer.cpp","modes/barrelracer.h"],
	copts=["-DBARRELRACER_TEST"],
	deps=[":mode",":teleop",":motion_profile"]
)

#cc_library(
#	name="auto",
#	srcs=["modes/auto.cpp"],
#	hdrs=["modes/auto.h"],
#	deps=[]
#)

#cc_binary(
#	name="auto_test",
#	srcs=["modes/auto.cpp","modes/auto.h"],
#	copts=["-DAUTO_TEST"],
#	deps=[]
#)

cc_library(
	name="delay",
	srcs=["modes/delay.cpp"],
	hdrs=["modes/delay.h"],
	deps=[":mode",":teleop",":auto_null",":auto_reach",":auto_statictwo",":auto_static",":portcullis",":cheval",":low_bar_low_score",":low_bar_wall_low_score",":low_bar_wall_high_score",":barrelracer"]
)

cc_binary(
	name="delay_test",
	srcs=["modes/delay.cpp","modes/delay.h"],
	copts=["-DDELAY_TEST"],
	deps=[
		":mode",":teleop",":auto_null",":auto_reach",":auto_statictwo",
		":auto_static",":portcullis",":cheval",":low_bar_low_score",
		":low_bar_wall_low_score",":low_bar_wall_high_score",":barrelracer"
	]
)

cc_library(
	name="low_bar_low_score",
	srcs=["modes/low_bar_low_score.cpp"],
	hdrs=["modes/low_bar_low_score.h"],
	deps=[":mode",":teleop"]
)

cc_binary(
	name="low_bar_low_score_test",
	srcs=["modes/low_bar_low_score.cpp","modes/low_bar_low_score.h"],
	copts=["-DLOW_BAR_LOW_SCORE_TEST"],
	deps=[":mode",":teleop"]
)

cc_library(
	name="mode",
	srcs=["modes/mode.cpp"],
	hdrs=["modes/mode.h"],
	deps=[":toplevel",":panel",":type"]
)

cc_binary(
	name="mode_test",
	srcs=["modes/mode.cpp","modes/mode.h"],
	copts=["-DMODE_TEST"],
	deps=[":toplevel",":panel",":type"]
)

cc_library(
	name="auto_statictwo",
	srcs=["modes/auto_statictwo.cpp"],
	hdrs=["modes/auto_statictwo.h"],
	deps=[":mode",":auto_stop",":teleop"]
)

cc_binary(
	name="auto_statictwo_test",
	srcs=["modes/auto_statictwo.cpp","modes/auto_statictwo.h"],
	copts=["-DAUTO_STATICTWO_TEST"],
	deps=[":mode",":teleop",":auto_stop"]
)

cc_library(
	name="auto_stop",
	srcs=["modes/auto_stop.cpp"],
	hdrs=["modes/auto_stop.h"],
	deps=[":mode",":teleop"]
)

cc_binary(
	name="auto_stop_test",
	srcs=["modes/auto_stop.cpp","modes/auto_stop.h"],
	copts=["-DAUTO_STOP_TEST"],
	deps=[":mode",":teleop"]
)

cc_library(
	name="auto_static",
	srcs=["modes/auto_static.cpp"],
	hdrs=["modes/auto_static.h"],
	deps=[":mode",":auto_stop"]
)

cc_binary(
	name="auto_static_test",
	srcs=["modes/auto_static.cpp","modes/auto_static.h"],
	copts=["-DAUTO_STATIC_TEST"],
	deps=[":mode",":auto_stop"]
)

cc_library(
	name="auto_null",
	srcs=["modes/auto_null.cpp"],
	hdrs=["modes/auto_null.h"],
	deps=[":mode",":teleop",":shooter_constants"]
)

cc_binary(
	name="auto_null_test",
	srcs=["modes/auto_null.cpp","modes/auto_null.h"],
	copts=["-DAUTO_NULL_TEST"],
	deps=[":mode",":teleop"]
)

cc_binary(
	name="step",
	srcs=["modes/step.cpp"],
	copts=["-DSTEP_TEST"],
	deps=[":mode",":teleop"]
)

cc_library(
	name="shooter_constants",
	srcs=["control/shooter_constants.cpp"],
	hdrs=["control/shooter_constants.h"],
	deps=[":interface",":shooter"]
)

cc_binary(
	name="shooter_constants_test",
	srcs=["control/shooter_constants.cpp","control/shooter_constants.h"],
	copts=["-DSHOOTER_CONSTANTS_TEST"],
	deps=[":interface",":shooter"]
)
