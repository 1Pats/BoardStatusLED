# Release Notes — BoardStatusLED V2.0

## New Status Modes

Eleven new built-in status modes have been added, covering a wider range of application states:

| Status     | Description                           |
|------------|---------------------------------------|
| WORKING    | Green, slow breathing effect          |
| ARMED      | Light red, very slow breathing effect |
| RETRIEVING | Sky blue, asymmetric blinking         |
| WAITING    | Yellow, very slow breathing effect    |
| RED        | Solid red                             |
| GREEN      | Solid green                           |
| BLUE       | Solid blue                            |
| YELLOW     | Solid yellow                          |
| PINK       | Solid pink                            |
| SKY        | Solid sky blue                        |
| WHITE      | Solid white                           |

## New Examples

- **Example0** added — a bare-minimum sketch showing the absolute simplest usage of the library (a single `LED(INFO)` call in `setup()`).

## Improvements

- **Broader board support:** The Supported Popular Boards table now includes ESP32-S3 successors.
- **Clearer setup instructions:** The Basic Usage section now explicitly describes how to define GPIO pins for external LEDs directly in `BoardStatusLEDSetup.h`, reducing the chance of misconfiguration.
- **Project structure corrected:** The example directory tree now accurately reflects the actual layout, with each example in its own subdirectory.

## Bug Fixes

- Minor fixes.

## Notes

- No breaking changes. Existing code using V1.x macros (`LED()`, `LEDUSER()`) is fully compatible.
- ESP32-C3 (single-core) remains unsupported.
