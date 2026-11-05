## English

MRoute09 is an unofficial firmware build for the **Polyend Tracker+**, based on Tracker CE v1.0.0. This is the first public MRoute release and includes the changes developed through MRoute09.

- **MIDI channel mapping.** In `Config → MIDI input map`, assign each incoming MIDI channel to Synth 1–3 or sample instrument 01–48. Play an external keyboard over USB or MIDI IN to record the mapped instrument and notes into a pattern.
- **Record synth knobs into pattern FX.** CC110–115 control Macros 1–6. CC116 controls delay send, CC117 reverb send, CC118 pan, and CC119 synth gain. Turn a knob in stopped REC to write at the cursor, or during live recording to write at the playhead. A second control uses the other FX column. Further movements of the same control update its value on that step. If both columns already contain other effects, they are preserved. Synth Gain has its own FX, independent of note velocity.
- **Perform mode.** External CC control now reaches synth macros and synth delay/reverb sends. The delay and reverb faders follow their corresponding inputs.
- **Pattern navigation.** Hold `More` and press `←` or `→` to jump a page of tracks (four or eight tracks, depending on the view).
- **Root-note pad guide.** In the Pattern Note column, dim LEDs mark the project root across octaves, including in REC. Toggle it in `Config → General → Root guide`. In Beat Slice, the guide follows the trigger notes of the loaded slices. MRoute09 lowers the selected slice LED so the current step is easier to distinguish from root and ordinary slices.

**Install on Tracker+** — Download `TrackerPlus_CE100_MRoute09.ptf`, copy it into `/Firmware/` on the microSD card, then select it in `Config → Firmware → Firmware update`. The patch is for developers and is not needed for installation.

## 한국어

MRoute09는 Tracker CE v1.0.0을 기반으로 만든 **Polyend Tracker+용 비공식 펌웨어**입니다. 첫 공개 릴리스로, MRoute09까지의 변경 사항을 모두 포함합니다.

- **MIDI 채널별 악기 지정** — `Config → MIDI input map`에서 각 입력 채널에 신스 1~3 또는 샘플 악기 01~48을 지정할 수 있습니다. USB나 MIDI IN으로 외부 건반을 연주하면 패턴에 노트와 해당 채널의 악기 번호가 함께 기록됩니다.
- **신스 노브를 패턴 FX로 녹음** — CC110~115는 매크로 1~6, CC116은 딜레이 센드, CC117은 리버브 센드, CC118은 팬, CC119는 신스 게인입니다. 정지 상태에서 REC를 켜면 커서 위치에, 라이브 녹음 중에는 재생 위치에 기록됩니다. 같은 스텝에서 같은 노브를 다시 돌리면 값이 갱신되고, 두 번째 노브는 남은 FX 칸을 사용합니다. 두 칸 모두 다른 FX가 있으면 건너뜁니다. 신스 게인은 노트 벨로시티와 별도의 FX입니다.
- **퍼폼 모드** — 외부 CC로 신스 매크로와 딜레이·리버브 센드를 조절할 수 있습니다. 화면의 딜레이·리버브 페이더도 각각의 입력을 따라갑니다.
- **트랙 페이지 이동** — 패턴에서 `More`를 누른 채 `←` 또는 `→`를 누르면 화면 구성에 따라 4트랙 또는 8트랙씩 이동합니다.
- **루트음 패드 가이드** — 패턴의 Note 열에서 옥타브마다 프로젝트 루트 위치가 약하게 켜지며 REC에서도 보입니다. `Config → General → Root guide`에서 켜고 끌 수 있습니다. Beat Slice에서는 실제 슬라이스가 있는 패드의 트리거 노트 번호를 기준으로 표시합니다. MRoute09에서는 선택한 슬라이스의 밝기를 낮춰 일반 슬라이스, 루트, 현재 스텝을 구분하기 쉽게 했습니다.

**Tracker+ 설치 방법** — `TrackerPlus_CE100_MRoute09.ptf`를 microSD 카드의 `/Firmware/`에 복사한 뒤 `Config → Firmware → Firmware update`에서 선택하세요. 기기에 설치할 때는 이 파일 하나만 필요합니다.

## Source / 소스

The attached cumulative patch applies to the upstream [Tracker CE v1.0.0 source](https://github.com/polyend/tracker-ce/tree/v1.0.0). 개발용 누적 패치는 원본 v1.0.0에서 다음과 같이 적용할 수 있습니다.

```sh
git apply tracker-ce-1.0.0-MRoute09.patch
make plus BUILD_TAG=MRoute09
```

Firmware SHA-256: `560b1c830db0c68886449f0a5b65929a4d0a1b319db897280edace1da913bf40`

Based on Polyend Tracker Community Edition. Copyright © 2020–2026 Polyend Sp. z o.o. and Polyend Tracker Community Edition Contributors. See the [license](https://github.com/polyend/tracker-ce/blob/v1.0.0/LICENSE), [notices](https://github.com/polyend/tracker-ce/blob/v1.0.0/NOTICE.md), and [synth/DSP library license](https://github.com/polyend/tracker-ce/blob/v1.0.0/synth/lib/LICENSE-PROPRIETARY.txt).
