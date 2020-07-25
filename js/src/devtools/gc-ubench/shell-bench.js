/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var FPS = 60;
var gNumSamples = 500;

// This requires a gHost to have been created that provides host-specific
// facilities. See eg spidermonkey.js.

loadRelativeToScript("argparse.js");
loadRelativeToScript("harness.js");
loadRelativeToScript("scheduler.js");
loadRelativeToScript("perf.js");
loadRelativeToScript("test_list.js");

var gPerf = new PerfTracker();

var tests = new Map();
foreach_test_file(f => loadRelativeToScript(f));
for (const [name, info] of tests.entries()) {
  if ("enabled" in info && !info.enabled) {
    tests.delete(name);
  }
}

function tick(loadMgr, timestamp) {
  gPerf.before_mutator(timestamp);
  gHost.start_turn();
  const events = loadMgr.tick(timestamp);
  gHost.end_turn();
  gPerf.after_mutator(timestamp);
  return events;
}

function report_events(events, loadMgr) {
  if (events & loadMgr.LOAD_ENDED) {
    print(`${loadMgr.lastActive.name} ended`);
  }
  if (events & loadMgr.LOAD_STARTED) {
    print(`${loadMgr.activeLoad().name} starting`);
  }
}

function run(opts, loads) {
  const sequence = [];
  for (const mut of loads) {
    if (tests.has(mut)) {
      sequence.push(mut);
    } else if (mut === "all") {
      sequence.push(...tests.keys());
    } else {
      sequence.push(...[...tests.keys()].filter(t => t.includes(mut)));
    }
  }
  if (loads.length === 0) {
    sequence.push(...tests.keys());
  }

  const loadMgr = new AllocationLoadManager(tests);
  const perf = new FrameHistory(gNumSamples);

  loadMgr.startCycle(sequence);
  const schedulerCtors = {
    keepup: OptimizeForFrameRate,
    vsync: VsyncScheduler,
  };
  const scheduler = new schedulerCtors[opts.sched](gPerf);

  perf.start();

  const t0 = gHost.now();

  let possible = 0;
  let frames = 0;
  while (loadMgr.load_running()) {
    const timestamp = gHost.now();
    const events = scheduler.tick(loadMgr, timestamp);
    const after_tick = gHost.now();

    perf.on_frame(timestamp);

    report_events(events, loadMgr);

    frames++;
    gPerf.handle_tick_events(events, loadMgr, timestamp, gHost.now());
    if (events & loadMgr.LOAD_ENDED) {
      possible += (loadMgr.testDurationMS / 1000) * FPS;
      const elapsed = ((after_tick - t0) / 1000).toFixed(2);
      print(`  observed ${frames} / ${possible} frames in ${elapsed} seconds`);
    }

    scheduler.wait_for_next_frame(t0, timestamp, after_tick);
  }
}

function report_results() {
  for (const result of gPerf.results) {
    const {
      load,
      elapsed_time,
      mutating,
      mutating_and_gc_fraction,
      suspended,
      full_time,
      frames,
      dropped_60fps_frames,
      dropped_60fps_fraction,
      minorGCs,
      majorGCs,
    } = result;

    const drop_pct = percent(dropped_60fps_fraction);
    const mut_pct = percent(mutating_and_gc_fraction);
    const mut_sec = mutating.toFixed(2);
    const full_sec = full_time.toFixed(2);
    const susp_sec = suspended.toFixed(2);
    print(`${load.name}:
  ${frames} (60fps) frames seen out of expected ${Math.floor(full_time * 60)}
  ${dropped_60fps_frames} = ${drop_pct} 60fps frames dropped
  ${mut_pct} of run spent mutating and GCing (${mut_sec}sec out of ${full_sec}sec vs ${susp_sec} sec waiting)
  ${minorGCs} minor GCs, ${majorGCs} major GCs
`);
  }
}

var argparse = new ArgParser("JS shell microbenchmark runner");
argparse.add_argument("--sched", {
  default: "keepup",
  options: ["keepup", "vsync"],
  help: "frame scheduler"
});
