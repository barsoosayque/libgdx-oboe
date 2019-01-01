## Examples

Here is a table of examples in this repository:

| Name | Description | Link |
|------|-------------|------|
| Sounds | An app which  contains two WAV sounds to show how is library working with sounds. You can select this assets and then use buttons to manage one specific soundpool. | [Go !](./examples/sounds) |
| Music | Simple audio player that you can use to manage tracks via streaming from file. It shows that OboeMusic works as expected. Again, you can select there some track which can be long or short, and control it with UI elements. | [Go !](./examples/music) |
| Async | Asynchronous loading and unloading Music and Sound instances using buttons. There you can also preview task progress. | [Go !](./examples/async) |

You can build each repository as a subproject within the repository, then upload APK with `adb`, and then launch it:

```bash
$ ../../gradlew assembleDebug
$ adb install build/output/apk/debug/*.apk
$ adb shell monkey -p barsoosayque.libgdxoboe.example_name_in_lowercase_here 1
```