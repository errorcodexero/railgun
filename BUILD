cc_test(
	name = "point_test",
	srcs = ["util/point.cpp","util/point.h","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"],
	copts = ["-DPOINT_TEST"],
	timeout="short"
)

cc_test(
	name = "type_test",
	srcs = ["util/type.cpp","util/type.h"],
	copts = ["-DTYPE_TEST"],
	timeout="short"

)

cc_library(
	name = "point",
	srcs = ["util/point.cpp"],
	hdrs = ["util/point.h","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"]
)

cc_test(
	name="util_test",
	#srcs=["util/util.cpp","util/interface.h","util/jag_interface.h","util/maybe.h","util/driver_station_interface.h","util/maybe_inline.h","util/checked_array.h","util/util.h","util/pwm.h"],
	srcs=["util/util.cpp"],
	copts=["-DUTIL_TEST"],
	deps=[":point"],
	timeout="short"

)

cc_library(
	name="util",
	srcs=["util/util.cpp"],
	deps=[":point"]
)

cc_test(
	name="jag_interface_test",
	srcs=["util/jag_interface.cpp"],
	copts=["-DJAG_INTERFACE_TEST"],
	deps=[":util"],
	timeout="short"

)

cc_library(name="jag_interface",srcs=["util/jag_interface.cpp"],deps=[":util"])

cc_test(
	name="driver_station_interface_test",
	srcs=["util/driver_station_interface.cpp"],
	copts=["-DDRIVER_STATION_INTERFACE_TEST"],
	deps=[":util"],
	timeout="short"

)

cc_library(
	name="driver_station_interface",
	srcs=["util/driver_station_interface.cpp"],
	deps=[":util"]
)

cc_test(
	name="interface_test",
	srcs=["util/interface.cpp"],
	copts=["-DINTERFACE_TEST"],
	deps=[":jag_interface",":driver_station_interface"],
	timeout="short"
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

cc_test(
	name="countup_timer_test",
	srcs=["util/countup_timer.cpp","util/countup_timer.h"],
	copts=["-DCOUNTUP_TIMER_TEST"],
	deps=[":interface"],
	timeout="short"

)

cc_library(
	name="countdown_timer",
	srcs=["util/countdown_timer.cpp"],
	hdrs=["util/countdown_timer.h"],
	deps=[":util"]
)

cc_test(
	name="countdown_timer_test",
	srcs=["util/countdown_timer.cpp","util/countdown_timer.h"],
	copts=["-DCOUNTDOWN_TIMER_TEST"],
	deps=[":util"],
	timeout="short"

)

cc_library(
	name="network",
	srcs=["util/network.cpp"],
	hdrs=["util/network.h"]
)

cc_test(
	name="network_test",
	srcs=["util/network.cpp","util/network.h"],
	copts=["-DNETWORK_TEST"],
	deps=[],
	timeout="short"

)

cc_library(
	name="negedge_trigger",
	srcs=["util/negedge_trigger.cpp"],
	hdrs=["util/negedge_trigger.h"]
)

cc_test(
	name="negedge_trigger_test",
	srcs=["util/negedge_trigger.cpp","util/negedge_trigger.h"],
	copts=["-DNEGEDGE_TRIGGER_TEST"],
	deps=[],
	timeout="short"

)

cc_library(
	name="nav2",
	srcs=["util/nav2.cpp"],
	hdrs=["util/nav2.h"],
	deps=[":nav"]
)

cc_test(
	name="nav2_test",
	srcs=["util/nav2.cpp","util/nav2.h"],
	copts=["-DNAV2_TEST"],
	deps=[":nav"],
	timeout="short"
)

cc_library(
	name="string_utils",
	srcs=["util/string_utils.cpp"],
	hdrs=["util/string_utils.h"]
)

cc_test(
	name="string_utils_test",
	srcs=["util/string_utils.cpp","util/string_utils.h"],
	copts=["-DSTRING_UTILS_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="posedge_trigger",
	srcs=["util/posedge_trigger.cpp"],
	hdrs=["util/posedge_trigger.h"]
)

cc_test(
	name="posedge_trigger_test",
	srcs=["util/posedge_trigger.cpp","util/posedge_trigger.h"],
	copts=["-DPOSEDGE_TRIGGER_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="debounce",
	srcs=["util/debounce.cpp"],
	hdrs=["util/debounce.h"],
	deps=[":countdown_timer"]
)

cc_test(
	name="debounce_test",
	srcs=["util/debounce.cpp","util/debounce.h"],
	copts=["-DDEBOUNCE_TEST"],
	deps=[":countdown_timer"],
	timeout="short"
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

cc_test(
	name="settable_toggle_test",
	srcs=["util/settable_toggle.cpp","util/settable_toggle.h"],
	copts=["-DSETTABLE_TOGGLE_TEST"],
	deps=[":posedge_trigger"],
	timeout="short"
)

cc_library(
	name="bound_tracker",
	srcs=["util/bound_tracker.cpp"],
	hdrs=["util/bound_tracker.h"]
)

cc_test(
	name="bound_tracker_test",
	srcs=["util/bound_tracker.cpp","util/bound_tracker.h"],
	copts=["-DBOUND_TRACKER_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="fixVictor",
	srcs=["util/fixVictor.cpp"],
	hdrs=["util/fixVictor.h"]
)

cc_test(
	name="fixVictor_test",
	srcs=["util/fixVictor.cpp","util/fixVictor.h"],
	copts=["-DFIXVICTOR_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="input",
	srcs=["util/input.cpp"],
	hdrs=["util/input.h"],
	deps=[":interface"]
)

cc_test(
	name="input_test",
	srcs=["util/input.cpp","util/input.h"],
	copts=["-DINPUT_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="nav",
	srcs=["util/nav.cpp"],
	hdrs=["util/nav.h"]
)

cc_test(
	name="nav_test",
	srcs=["util/nav.cpp","util/nav.h"],
	copts=["-DNAV_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="output",
	srcs=["util/output.cpp"],
	hdrs=["util/output.h"],
	deps=[":interface"]
)

cc_test(
	name="output_test",
	srcs=["util/output.cpp","util/output.h"],
	copts=["-DOUTPUT_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="motion_profile",
	srcs=["util/motion_profile.cpp"],
	hdrs=["util/motion_profile.h"]
)

cc_test(
	name="motion_profile_test",
	srcs=["util/motion_profile.cpp","util/motion_profile.h"],
	copts=["-DMOTION_PROFILE_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="perf_tracker",
	srcs=["util/perf_tracker.cpp"],
	hdrs=["util/perf_tracker.h"],
	deps=[":interface"]
)

cc_test(
	name="perf_tracker_test",
	srcs=["util/perf_tracker.cpp","util/perf_tracker.h"],
	copts=["-DPERF_TRACKER_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="posedge_trigger_debounce",
	srcs=["util/posedge_trigger_debounce.cpp"],
	hdrs=["util/posedge_trigger_debounce.h"],
	deps=[":debounce",":posedge_trigger"]
)

cc_test(
	name="posedge_trigger_debounce_test",
	srcs=["util/posedge_trigger_debounce.cpp","util/posedge_trigger_debounce.h"],
	copts=["-DPOSEDGE_TRIGGER_DEBOUNCE_TEST"],
	deps=[":debounce",":posedge_trigger"],
	timeout="short"
)

cc_library(
	name="meta",
	srcs=["util/meta.cpp"],
	hdrs=["util/meta.h"],
	deps=[":interface",":tilt"]
)

cc_test(
	name="meta_test",
	srcs=["util/meta.cpp","util/meta.h"],
	copts=["-DMETA_TEST"],
	deps=[":interface",":tilt",":front"],
	timeout="short"
)

cc_library(
	name="monitor",
	srcs=["control/monitor.cpp"],
	hdrs=["control/monitor.h"],
	deps=[":main"]
)

cc_test(
	name="monitor_test",
	srcs=["control/monitor.cpp","control/monitor.h"],
	copts=["-DMONITOR_TEST"],
	deps=[":main"],
	timeout="short"
)

cc_library(
	name="motor_check",
	srcs=["control/motor_check.cpp"],
	hdrs=["control/motor_check.h"],
	deps=[":util"]
)

cc_test(
	name="motor_check_test",
	srcs=["control/motor_check.cpp","control/motor_check.h"],
	copts=["-DMOTOR_CHECK_TEST"],
	deps=[":util"],
	timeout="short"
)

cc_library(
	name="main",
	srcs=["control/main.cpp"],
	hdrs=["control/main.h"],
	deps=[":force_interface",":posedge_toggle",":perf_tracker",":countdown_timer",":countup_timer",":toplevel",":panel",":nav",":nav2",":log",":posedge_trigger_debounce",":motion_profile",":tilt_presets",":shooter_constants",":executive_impl"]
)

cc_test(
	name="main_test",
	srcs=["control/main.cpp","control/main.h"],
	copts=["-DMAIN_TEST"],
	deps=[":force_interface",":toplevel",":posedge_toggle",":perf_tracker",":countup_timer",":panel",":nav2",":log",":posedge_trigger_debounce",":motion_profile",":tilt_presets",":monitor",":shooter_constants"],
	timeout="short"
)

cc_library(
	name="log",
	srcs=["control/log.cpp"],
	hdrs=["control/log.h"],
	deps=[":interface",":toplevel"]
)

cc_test(
	name="log_test",
	srcs=["control/log.cpp","control/log.h","control/formal.h"],
	copts=["-DLOG_TEST"],
	deps=[":interface",":toplevel"],
	timeout="short"
)

cc_library(
	name="drivebase",
	srcs=["control/drivebase.cpp"],
	hdrs=["control/drivebase.h","util/quick.h"],
	deps=[":interface",":motor_check",":countdown_timer",":fixVictor"]
)

cc_test(
	name="drivebase_test",
	srcs=["control/drivebase.cpp","control/drivebase.h","util/quick.h","control/formal.h"],
	copts=["-DDRIVEBASE_TEST"],
	deps=[":interface",":motor_check",":countdown_timer",":fixVictor"],
	timeout="short"
)

cc_library(
	name="climb_release",
	srcs=["control/climb_release.cpp"],
	hdrs=["control/climb_release.h"],
	deps=[":countdown_timer"]
)

cc_test(
	name="climb_release_test",
	srcs=["control/climb_release.cpp","control/climb_release.h","control/formal.h"],
	copts=["-DCLIMB_RELEASE_TEST"],
	deps=[":countdown_timer",":interface"],
	timeout="short"
)

cc_library(
	name="force",
	srcs=["control/force.cpp"],
	hdrs=["control/force.h"],
	deps=[":interface"]
)

cc_library(
	name="force_interface",
	srcs=["control/force_interface.cpp"],
	hdrs=["control/force_interface.h"],
	deps=[":posedge_trigger",":force"]
)

cc_test(
	name="force_interface_test",
	srcs=["control/force_interface.cpp","control/force_interface.h"],
	copts=["-DFORCE_INTERFACE_TEST"],
	deps=[":posedge_trigger",":force",":interface"],
	timeout="short"
)

cc_library(
	name="toplevel",
	srcs=["control/toplevel.cpp"],
	hdrs=["control/toplevel.h"],
	deps=[":pump",":drivebase",":climb_release",":winch",":collector",":shooter",":input"]
)

cc_test(
	name="toplevel_test",
	srcs=["control/toplevel.cpp","control/toplevel.h","control/formal.h"],
	copts=["-DTOPLEVEL_TEST"],
	deps=[":pump",":drivebase",":climb_release",":winch",":collector",":shooter",":input",":output"],
	timeout="short"
)

cc_library(
	name="tilt",
	srcs=["control/tilt.cpp"],
	hdrs=["control/tilt.h","util/quick.h"],
	deps=[":util",":countdown_timer"]
)

cc_test(
	name="tilt_test",
	srcs=["control/tilt.cpp","control/tilt.h","util/quick.h","control/formal.h"],
	copts=["-DTILT_TEST"],
	deps=[":util",":countdown_timer",":interface"],
	timeout="short"
)

cc_library(
	name="tilt_presets",
	srcs=["control/tilt_presets.cpp"],
	hdrs=["control/tilt_presets.h"],
	deps=[]
)

cc_test(
	name="tilt_presets_test",
	srcs=["control/tilt_presets.cpp","control/tilt_presets.h"],
	copts=["-DTILT_PRESETS_TEST"],
	deps=[],
	timeout="short"
)

cc_library(
	name="sim",
	srcs=["control/sim.cpp"],
	deps=[]
)

cc_test(
	name="sim_test",
	srcs=["control/sim.cpp"],
	copts=["-DSIM_TEST"],
	deps=[":toplevel",":type"],
	timeout="short"
)

cc_library(
	name="sides",
	srcs=["control/sides.cpp"],
	hdrs=["control/sides.h"],
	deps=[":interface",":nop"]
)

cc_test(
	name="sides_test",
	srcs=["control/sides.cpp","control/sides.h","control/formal.h"],
	copts=["-DSIDES_TEST"],
	deps=[":interface",":nop"],
	timeout="short"
)

cc_library(
	name="front",
	srcs=["control/front.cpp"],
	hdrs=["control/front.h"],
	deps=[":interface"]
)

cc_test(
	name="front_test",
	srcs=["control/front.cpp","control/front.h","control/formal.h"],
	copts=["-DFRONT_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="pump",
	srcs=["control/pump.cpp"],
	hdrs=["control/pump.h","util/maybe.h"],
	deps=[":interface"]
)

cc_test(
	name="pump_test",
	srcs=["control/pump.cpp","control/pump.h","util/maybe.h","control/formal.h"],
	copts=["-DPUMP_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="tote_sensors",
	srcs=["control/tote_sensors.cpp"],
	hdrs=["control/tote_sensors.h"],
	deps=[":nop"]
)

cc_test(
	name="tote_sensors_test",
	srcs=["control/tote_sensors.cpp","control/tote_sensors.h","control/formal.h"],
	copts=["-DTOTE_SENSORS_TEST"],
	deps=[":nop"],
	timeout="short"
)

cc_library(
	name="nop",
	srcs=["control/nop.cpp"],
	hdrs=["control/nop.h"],
	deps=[":interface"]
)

cc_test(
	name="nop_test",
	srcs=["control/nop.cpp","control/nop.h","control/formal.h"],
	copts=["-DNOP_TEST"],
	deps=[":interface"],
	timeout="short"
)

cc_library(
	name="collector",
	srcs=["control/collector.cpp"],
	hdrs=["control/collector.h"],
	deps=[":front",":sides",":tilt"]
)

cc_test(
	name="collector_test",
	srcs=["control/collector.cpp","control/collector.h","control/formal.h"],
	copts=["-DCOLLECTOR_TEST"],
	deps=[":front",":sides",":tilt"],
	timeout="short"
)

cc_library(
	name="shooter",
	srcs=["control/shooter.cpp"],
	hdrs=["control/shooter.h","util/quick.h"],
	deps=[":interface",":countdown_timer"]
)

cc_test(
	name="shooter_test",
	srcs=["control/shooter.cpp","control/shooter.h","control/formal.h","util/quick.h"],
	copts=["-DSHOOTER_TEST"],
	deps=[":countdown_timer",":interface"],
	timeout="short"

)

cc_library(
	name="winch",
	srcs=["control/winch.cpp"],
	hdrs=["control/winch.h"],
	deps=[":interface",":countdown_timer"]
)

cc_test(
	name="winch_test",
	srcs=["control/winch.cpp","control/winch.h","control/formal.h"],
	copts=["-DWINCH_TEST"],
	deps=[":interface",":countdown_timer"],
	timeout="short"
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

cc_test(
	name="portcullis_test",
	srcs=["executive/portcullis.cpp","executive/portcullis.h"],
	copts=["-DPORTCULLIS_TEST"],
	deps=[":executive",":executive_impl",":tilt_presets",":test"],
	timeout="short"
)

cc_library(
	name="executive_impl",
	srcs=[
		"executive/delay.cpp",
		"executive/auto_stop.cpp",
		"executive/low_bar_low_score.cpp",
		"executive/low_bar_wall_high_score.cpp",
		"executive/low_bar_wall_low_score.cpp",
		"executive/cheval.cpp",
		"executive/portcullis.cpp",
		"executive/auto_static.cpp",
		"executive/auto_statictwo.cpp",
		"executive/auto_reach.cpp",
		"executive/auto_null.cpp",
		"executive/teleop.cpp",
		"executive/barrelracer.cpp"
	],
	hdrs=[
		"executive/delay.h",
		"executive/auto_stop.h",
		"executive/low_bar_low_score.h",
		"executive/low_bar_wall_high_score.h",
		"executive/low_bar_wall_low_score.h",
		"executive/cheval.h",
		"executive/portcullis.h",
		"executive/auto_static.h",
		"executive/auto_statictwo.h",
		"executive/auto_reach.h",
		"executive/auto_null.h",
		"executive/teleop.h",
		"executive/barrelracer.h"
	],
	deps=[":executive",":posedge_trigger_debounce",":posedge_toggle",":tilt_presets",":shooter_constants",":motion_profile"]
)

cc_test(
	name="auto_reach_test",
	srcs=["executive/auto_reach.cpp","executive/auto_reach.h"],
	copts=["-DAUTO_REACH_TEST"],
	deps=[":executive",":executive_impl",":test"],
	timeout="short"
)

cc_test(
	name="cheval_test",
	srcs=["executive/cheval.cpp","executive/cheval.h"],
	copts=["-DCHEVAL_TEST"],
	deps=[":executive",":test"],
	timeout="short"
)

cc_test(
	name="low_bar_wall_high_score_test",
	srcs=["executive/low_bar_wall_high_score.cpp","executive/low_bar_wall_high_score.h"],
	copts=["-DLOW_BAR_WALL_HIGH_SCORE_TEST"],
	deps=[":executive",":test"],
	timeout="moderate"
)

cc_test(
	name="teleop_test",
	srcs=["executive/teleop.cpp","executive/teleop.h"],
	copts=["-DTELEOP_TEST"],
	deps=[":executive",":posedge_trigger_debounce",":posedge_toggle",":tilt_presets",":shooter_constants",":test"],
	size="medium"
)

cc_test(
	name="low_bar_wall_low_score_test",
	srcs=["executive/low_bar_wall_low_score.cpp","executive/low_bar_wall_low_score.h"],
	copts=["-DLOW_BAR_WALL_LOW_SCORE_TEST"],
	deps=[":executive",":test"],
	size="medium"
)

cc_test(
	name="barrelracer_test",
	srcs=["executive/barrelracer.cpp","executive/barrelracer.h"],
	copts=["-DBARRELRACER_TEST"],
	deps=[":executive",":motion_profile",":test"],
	timeout="short"
)

cc_test(
	name="delay_test",
	srcs=["executive/delay.cpp","executive/delay.h"],
	copts=["-DDELAY_TEST"],
	deps=[
		":executive",":executive_impl",
		":test"
	],
	timeout="short"
)

cc_test(
	name="low_bar_low_score_test",
	srcs=["executive/low_bar_low_score.cpp","executive/low_bar_low_score.h"],
	copts=["-DLOW_BAR_LOW_SCORE_TEST"],
	deps=[":test"],
	timeout="short"
)

cc_library(
	name="executive",
	srcs=["executive/executive.cpp"],
	hdrs=["executive/executive.h"],
	deps=[":toplevel",":panel",":type"]
)

cc_test(
	name="executive_test",
	srcs=["executive/executive.cpp","executive/executive.h"],
	copts=["-DEXECUTIVE_TEST"],
	deps=[":executive_impl",":panel",":type"],
	timeout="short"
)

cc_test(
	name="auto_statictwo_test",
	srcs=["executive/auto_statictwo.cpp","executive/auto_statictwo.h"],
	copts=["-DAUTO_STATICTWO_TEST"],
	deps=[":test"],
	timeout="short"
)

cc_test(
	name="auto_stop_test",
	srcs=["executive/auto_stop.cpp","executive/auto_stop.h"],
	copts=["-DAUTO_STOP_TEST"],
	deps=[":test"],
	timeout="short"
)

cc_test(
	name="auto_static_test",
	srcs=["executive/auto_static.cpp","executive/auto_static.h"],
	copts=["-DAUTO_STATIC_TEST"],
	deps=[":test"],
	timeout="short"
)

cc_library(
	name="test",
	srcs=["executive/test.cpp"],
	hdrs=["executive/test.h"],
	deps=[":executive",":executive_impl"]
)

cc_test(
	name="auto_null_test",
	srcs=["executive/auto_null.cpp","executive/auto_null.h"],
	copts=["-DAUTO_NULL_TEST"],
	deps=[":executive_impl",":test"],
	timeout="short"
)

cc_test(
	name="step_test",
	srcs=["executive/step.cpp","executive/step.h"],
	copts=["-DSTEP_TEST"],
	deps=[":executive_impl"],
	timeout="short"
)

cc_library(
	name="shooter_constants",
	srcs=["control/shooter_constants.cpp"],
	hdrs=["control/shooter_constants.h"],
	deps=[":interface",":shooter"]
)

cc_test(
	name="shooter_constants_test",
	srcs=["control/shooter_constants.cpp","control/shooter_constants.h"],
	copts=["-DSHOOTER_CONSTANTS_TEST"],
	deps=[":interface",":shooter"],
	timeout="short"
)
