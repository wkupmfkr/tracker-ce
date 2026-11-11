# JHNY_Trkr25 — Unofficial Tracker+ Community Edition

Based on upstream Tracker CE `v1.0.0`. This release supersedes PolySteps16 in this fork. **Tracker+ only.** Download `TrackerPlus_JHNY_Trkr25.ptf`; keep the `TrackerPlus_` filename prefix for device recognition.

## English

### Changes since the last public release, PolySteps16

- **Independent playback markers:** black playback cells follow each track's actual step. Tracks with a different effective loop length or a rate other than ×1 also show a thin, fixed-green underline. Works in both track banks and vertical/horizontal views. Loop-track headers start with the track number and length/current position, followed by up to seven characters of a custom name.
- **Per-track playback rate:** ×0.25, ×0.5, ×0.75, ×1, ×1.5, ×2 and ×4 for tracks 1–16. Select a track, hold **Step jump first, then Shift**, and use the encoder/arrows. During playback, changes apply at the next master bar and restart the affected tracks at step 1; `*` means pending. Settings are stored per pattern and preserved by full-pattern Copy/Paste and Duplicate. Duplicate extends the master pattern while preserving individual loop lengths/rates. Master BPM, MIDI clock, metronome and Song progression remain unchanged. Local note/FX timing and live-record positions follow the track rate; this does not change sample playback pitch. Applies to normal Pattern/Song playback; selection playback, Preview and Perform's alternate playback modes retain their original ×1 timing.
- **Rotate steps:** in Pattern REC mode, hold **More + Up/Down** to rotate steps earlier/later with wraparound. Without a range selection, uses the track's loop length; range selections use their selected rows. Complete steps move together, including note, instrument, both FX, volume and microtiming. Supports Undo. Existing More + Left/Right track paging remains available.
- **Sample Editor Tempo Grid:** **Shift + Zoom** toggles a visual grid based on project BPM. It follows zoom/pan and thins at wide zoom levels. This is a visual guide, without snapping, tempo detection or time-stretching. Off by default; session-only.
- **Compact Song repeats:** with REC off, the right-hand playlist groups consecutive identical patterns, e.g. `P1 x4`, with an occurrence indicator for the selected group. REC expands individual slots for editing. Navigation and playback still use the original song slots.
- **Chromatic pad guide:** alongside brighter root notes, dimly lights the other six major-scale degrees relative to the selected root. Recorded/pressed-note indications take priority. Honors Root guide On/Off in record and non-record modes; Beat Slice keeps its slice-number guide.
- **C3 sample/synth alignment:** newly imported WAVs in empty slots use Tracker C3 for original-rate playback. Existing instruments/projects keep their pitch; WAV replacement in a populated slot and PTI import retain the instrument's root. Beat Slice and Wavetable behavior is preserved. Sample Editor/Playback auditions follow the new root. Internal synth labels and onboard pads now agree at C3 with external MIDI 60, while existing stored synth notes retain their sound. Set **Config → MIDI → Middle C = C3** for corresponding external sample input; the setting is not changed automatically. Older firmware does not understand the new sample-root metadata, so new C3 instruments can play at a different pitch after downgrading.
- **Live recording starts Unarmed:** all 16 tracks start Unarmed. In Pattern, hold **Shift and press Rec**, then use track soft buttons to arm the desired tracks; use the other bank for tracks 9–16. Multiple tracks can be armed. Choices remain during the session, reset on restart and are not saved in projects. Stopped step entry remains unchanged.
- **Startup initialization correction:** removed an unsafe pre-SDRAM initialization introduced in the withdrawn MRoute17 development build and added a compile-time guard. This code defect was verified; it is not proof of the cause of the earlier physical-device recovery incident.

### Retained from PolySteps16

Independent track lengths and bar-based reset; multi-solo across tracks 1–16; MIDI channel-to-instrument routing and note/CC recording; root/slice guides; the Sample / Synth 1 / Synth 2 / Synth 3 / Dry / Delay / Reverb / Line In global mixer; unified CC14 Delay and CC15 Reverb sends (synth CC116/117 aliases retained); global return CC79/80; and the FM tuner timeout fix. Performance Scenes and Mute Banks are not included.

### Installation, source and validation

Back up projects/instruments before updating. Copy the PTF into the card's `/Firmware/` folder and select it in **Config → Firmware → Firmware update**. This is a custom community build, not an official Polyend firmware release.

- `TrackerPlus_JHNY_Trkr25.ptf`: the exact locally tested build, not rebuilt for publication.
- `JHNY_Trkr25.patch`: cumulative source patch against clean upstream `v1.0.0`; do not stack it on an earlier cumulative patch.
- `RELEASE-NOTES.md`: this English/Korean guide. `CHANGELOG-JHNY_Trkr25.md`: the small change from Trkr24 to Trkr25.
- `validation.json`: automated checks and separately attributed user confirmations. `SHA256SUMS.txt`: attachment checksums.
- Full source is on branch `jhny-trkr25` and tag `JHNY_Trkr25`. Build with ARM GCC 10.3.1: `make plus NPROC=12 BUILD_TAG=JHNY_Trkr25`.

Build, targeted ARM startup/arm-toggle checks, PTF/ELF consistency and cumulative patch checks passed. Earlier development revisions also passed scoped timing, rendering, import/serialization and mocked mode-transition tests. The owner confirmed on-device per-track rate operation during development, C3 sample playback, matching synth pad/MIDI pitch, and, on Trkr25, Unarmed startup and recording only to chosen armed tracks. These are user reports, not a full hardware test suite. Long-duration stress, maximum-rate load and instrumented external MIDI timing remain unverified. The startup audit is targeted, not a complete hardware boot simulation; existing compiler warnings remain.

## 한국어

### 마지막 공개판 PolySteps16 이후 변경 사항

- **트랙별 재생 표시:** 검정 재생 셀이 각 트랙의 실제 스텝을 따라갑니다. 길이가 기본 패턴과 다르거나 속도가 ×1이 아닌 트랙에는 얇은 녹색 밑줄도 표시합니다. 1–16번 트랙과 세로/가로 화면을 지원합니다. 루프 트랙 제목은 트랙 번호와 길이/현재 위치부터 표시하며 사용자 이름은 뒤에 최대 7글자를 붙입니다.
- **트랙별 배속:** ×0.25, ×0.5, ×0.75, ×1, ×1.5, ×2, ×4. 트랙 선택 후 **Step jump를 먼저 누른 채 Shift**, 노브/방향키로 조절합니다. 재생 중 변경은 다음 마디에 반영되며 해당 트랙이 1스텝부터 시작합니다. `*`는 적용 대기입니다. 패턴별로 저장되고 전체 Copy/Paste와 Duplicate에도 유지됩니다. Duplicate는 전체 패턴 길이를 늘리되 개별 길이·배속은 유지합니다. 마스터 BPM·MIDI 클럭·메트로놈·Song 진행은 그대로이고, 로컬 노트/FX와 녹음 위치가 배속을 따릅니다. 샘플 자체의 피치는 바꾸지 않습니다. 일반 Pattern/Song에 적용되며 선택 재생·Preview·Perform의 별도 재생 모드는 기존 ×1입니다.
- **Rotate:** Pattern REC에서 **More + 위/아래**로 스텝을 앞/뒤로 순환 이동합니다. 영역 선택이 없으면 트랙 루프 길이, 선택이 있으면 선택 범위를 사용합니다. 노트·악기·두 FX·볼륨·마이크로타이밍이 함께 이동하며 Undo를 지원합니다. More + 좌/우 트랙 페이지 이동도 유지됩니다.
- **Sample Editor Tempo Grid:** **Shift + Zoom**으로 프로젝트 BPM 기준의 시각적 격자를 켜고 끕니다. 줌/이동을 따라가며 촘촘하면 간격을 넓힙니다. 스냅·템포 감지·타임스트레치는 하지 않습니다. 기본 꺼짐이며 전원 세션 동안만 유지됩니다.
- **Song 반복 묶음:** REC가 꺼져 있으면 오른쪽 목록의 연속된 같은 패턴을 `P1 x4`처럼 묶고 선택된 반복 위치를 표시합니다. REC를 켜면 개별 행으로 펼칩니다. 실제 이동과 재생 순서는 기존 송 슬롯 기준입니다.
- **Chromatic 패드 가이드:** 밝은 루트 외에 루트 기준 장음계의 나머지 여섯 음도 어둡게 켭니다. 기록된 음/누른 패드가 우선하며 Root guide 설정과 녹음/비녹음 상태를 따릅니다. Beat Slice의 슬라이스 번호 가이드는 유지됩니다.
- **샘플·신스 C3 정렬:** 빈 슬롯에 새로 넣는 WAV는 Tracker C3에서 원음으로 재생됩니다. 기존 곡과 악기 음높이는 유지하고, 사용 중인 슬롯의 WAV 교체와 PTI 로드는 악기 기준을 보존합니다. Beat Slice·Wavetable은 기존대로입니다. Sample Editor/Playback 미리듣기에도 새 기준을 적용합니다. 신스는 본체 패드·패턴 표시·외부 MIDI 60 입력이 C3로 맞으며 기존 저장 노트의 소리는 유지합니다. 외부 샘플 입력도 맞추려면 **Config → MIDI → Middle C = C3**로 설정하세요. 자동 변경하지 않습니다. 이전 펌웨어는 새 루트 정보를 해석하지 못하므로 다운그레이드하면 새 C3 악기의 음높이가 달라질 수 있습니다.
- **라이브 녹음 기본 Unarmed:** 16트랙 모두 Unarmed로 시작합니다. Pattern에서 **Shift를 누른 채 Rec → 원하는 트랙 하단 버튼**으로 암드합니다. 9–16번은 해당 뱅크에서 선택하며 다중 암드가 가능합니다. 선택은 사용 중 유지되지만 재시작하면 초기화되며 프로젝트에는 저장하지 않습니다. 정지 스텝 입력은 그대로입니다.
- **시작 초기화 수정:** 철회한 개발판 MRoute17에 있던 SDRAM 준비 전 초기화 위험을 제거하고 재발 방지 컴파일 검사를 추가했습니다. 코드 결함은 확인됐지만 과거 실기기 복구 문제의 원인이라고 확정한 것은 아닙니다.

### 유지되는 기능

트랙별 길이·마디 리셋, 1–16번 다중 솔로, MIDI 채널별 악기 매핑과 노트/CC 녹음, 루트·슬라이스 가이드, Sample / Synth 1 / Synth 2 / Synth 3 / Dry / Delay / Reverb / Line In 믹서, CC14 딜레이·CC15 리버브 센드(신스 CC116/117 유지), 글로벌 리턴 CC79/80, FM 튜너 대기 제한을 유지합니다. 퍼폼 씬과 뮤트 뱅크는 포함하지 않습니다.

### 설치·소스·확인 범위

프로젝트와 악기를 백업한 뒤 PTF를 SD카드 `/Firmware/`에 넣고 **Config → Firmware → Firmware update**에서 선택하세요. Tracker+ 전용 비공식 커뮤니티 빌드입니다. 파일명 앞의 `TrackerPlus_`를 유지하세요.

첨부 PTF는 현재 확인된 파일 그대로이며 공개를 위해 재빌드하지 않았습니다. 패치는 upstream `v1.0.0` 기준 누적본이므로 이전 패치 위에 중복 적용하지 않습니다. 전체 소스는 `jhny-trkr25` 브랜치와 `JHNY_Trkr25` 태그에 있습니다. ARM GCC 10.3.1에서 `make plus NPROC=12 BUILD_TAG=JHNY_Trkr25`로 빌드합니다.

빌드·제한된 ARM 초기화/암드 전환·PTF/ELF 일치·패치 적용 검사를 통과했습니다. 이전 개발 단계에서 배속·표시·샘플 가져오기/저장 형식·모의 화면 전환 검사도 수행했습니다. 사용자는 개발 과정에서 배속, 샘플 C3 원음, 신스 본체 패드와 MIDI의 표시/음높이 일치를 확인했고, Trkr25에서 기본 Unarmed와 선택한 트랙 녹음을 확인했습니다. 이는 사용자 실기기 확인이며 전 기능 검증을 의미하지 않습니다. 장시간 스트레스·최대 배속 부하·계측한 외부 MIDI 타이밍은 미검증입니다. 초기화 감사는 전체 하드웨어 부팅 시뮬레이션이 아니며 기존 컴파일 경고도 남아 있습니다.
