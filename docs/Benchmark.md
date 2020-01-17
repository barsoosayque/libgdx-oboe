## Benchmark

You can find benchmark code in [~/benchmark](../benchmark). It uses it's own assets.
Benchmark names follow pattern "**ACTION***IMPLEMENTATION*" (e.g. *loadSoundsGDX*, which means that action is **loadSounds** and it uses default libGDX android audio).

<!--  TODO: place benchmark results here -->

### Legend:
- *loadSounds*: load 5 second files as Sound of each supported format both stereo and mono (6 files total).
- *loadMusic*: load 30 second files as Music of each supported format (3 files total).
- *seekMusic*: seek every 5 seconds of Music files forward to the end, and then backward to the start.

### Running

To run benchmarks, connect a physical device and use `benchmark:run` gradle command, i.e.:

```sh
$ ./gradlew benchmark:run
# optionaly run a script to generate HTML report of the run
$ ./ sh generate_charts.sh
```

Keep the screen on while benchmark is running, otherwise it will fail. Also make sure that your device is charged.
Benchmark will generate JSON report in Downloads directory on your device, then copy it in `~/benchmark/build/libgdx-oboe-report` on the host machine (where you run gradle command).
You can then execute `generate_charts.sh` shell script to make a HTML page with charts to compare Gdx implementation with Oboe implementation.

Running benchmarks takes a lot of time, this is normal.
To get more accurate results on rooted devices, you may consider (locking clocks)[https://developer.android.com/studio/profile/run-benchmarks-in-ci#clock-locking].
