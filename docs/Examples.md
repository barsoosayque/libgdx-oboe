## Examples

Here is a table of examples in this repository:

| Name | Description | Link |
|------|-------------|------|
| Sounds | An app which  contains two WAV sounds to show how is library working with sounds. You can select this assets and then use buttons to manage one specific soundpool. | [Go !](./samples/sounds) |

You can build each repository as a subproject within the repository, then upload APK with `adb`, and then launch it:

```bash
$ ../../gradlew assembleDebug
$ adb install build/output/apk/debug/*.apk
$ adb shell monkey -p barsoosayque.libgdxoboe.example_name_in_lowercase_here 1
```