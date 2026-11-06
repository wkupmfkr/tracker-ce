# PolySteps 16 · Tracker+ Community Edition 1.0.0

## English

PolySteps 16 supersedes the MRoute09 release for Tracker+. Install `TrackerPlus_PolySteps16.ptf`. The device displays **MRoute16** because the download is the same firmware image previously used under that name.

### Changes since MRoute09

- **Sample Recorder and FM radio.** Opening the recorder could freeze the controls while the FM tuner waited for a clock response. The wait now has a timeout. This addresses one identified cause of freezes when switching modes.
- **Independent track lengths.** In Pattern, select a track, hold **Length first, then Shift**, and turn the wheel. A 7-step melody can loop against a 16-step drum track. Set the length back to the pattern length to follow the pattern again. Independent lengths apply during normal Pattern and Song playback.
- **Bring the tracks back together.** With Length and Shift held, use Left/Right to set the automatic reset interval in bars. R0 disables it, R1–R64 sets the interval. During playback, press Insert with both held to bring all tracks to step 1 on the next bar.
- **Multi-solo across tracks 1–16.** Hold Shift and tap a screen key to mute its track on release. Hold the key longer to toggle solo. Add tracks from either bank and hold again to remove them. Clearing the last solo restores the previous mute mix.
- **Global Mixer 3/3.** The eight faders are Sample, Synth 1, Synth 2, Synth 3, Dry, Delay, Reverb and Line In. Dry controls the combined dry sample and synth signal. The mixer supports multi-solo.
- **MIDI sends.** CC14 controls Delay Send and CC15 controls Reverb Send on both sample and synth instruments. Synth CC116/117 still control Delay/Reverb. CC79/80 control the global Delay/Reverb returns.

**Display note.** During polymetric playback, the track header shows the current step and loop length. The small track marker and counter can occasionally appear to pause. The main playhead follows the pattern.

Copy the PTF to `/Firmware/` on the microSD card and select it in **Config → Firmware → Firmware update**. The accompanying source patch applies to upstream `v1.0.0`.

## 한국어

PolySteps 16은 기존 Tracker+용 MRoute09의 후속 공개판입니다. 설치 파일은 `TrackerPlus_PolySteps16.ptf`이며, 기기에는 기존 빌드명 **MRoute16**이 표시됩니다.

### MRoute09 이후 변경 사항

- **샘플 레코더와 FM 라디오.** 레코더에 들어갈 때 FM 튜너의 클록 응답을 계속 기다려 버튼까지 멈출 수 있던 경로에 대기 시간 제한을 넣었습니다.
- **트랙별 길이.** 패턴에서 트랙을 선택하고 **Length를 먼저 누른 채 Shift**를 누른 다음 휠을 돌립니다. 16스텝 드럼 위에 7스텝 멜로디를 반복할 수 있습니다. 길이를 패턴 길이로 되돌리면 다시 패턴을 따라갑니다. 일반 패턴·송 재생에서 동작합니다.
- **마디 단위 리셋.** 두 버튼을 누른 채 좌우 화살표로 자동 리셋 간격을 고릅니다. R0은 끄기, R1–R64는 마디 수입니다. 재생 중 두 버튼을 누른 채 Insert를 누르면 다음 마디 첫 박에 모든 트랙이 1스텝으로 돌아갑니다.
- **1–16번 트랙 다중 솔로.** Shift를 누른 채 화면 아래 트랙 버튼을 짧게 눌렀다 떼면 뮤트, 길게 누르면 솔로입니다. 두 뱅크를 오가며 여러 트랙을 추가할 수 있고 같은 버튼을 다시 길게 누르면 해제됩니다. 마지막 솔로를 풀면 기존 뮤트 조합으로 돌아갑니다.
- **글로벌 믹서 3/3.** Sample, Synth 1, Synth 2, Synth 3, Dry, Delay, Reverb, Line In 순서입니다. Dry는 샘플과 신스의 원음을 함께 조절합니다. 믹서에서도 다중 솔로를 지원합니다.
- **MIDI 센드.** 샘플과 신스 모두 CC14는 딜레이 센드, CC15는 리버브 센드입니다. 신스는 기존 CC116/117로도 딜레이·리버브 센드를 조절할 수 있습니다. 글로벌 딜레이·리버브 리턴은 CC79/80입니다.

**화면 표시.** 폴리메트릭 재생 중 트랙 헤더에 현재 스텝과 길이가 나옵니다. 작은 트랙 마커와 숫자가 간헐적으로 멈춰 보일 수 있으며, 기본 재생 줄은 전체 패턴 위치를 따라갑니다.

설치 파일을 microSD 카드의 `/Firmware/`에 복사하고 **Config → Firmware → Firmware update**에서 선택합니다. 첨부된 누적 소스 패치는 원본 `v1.0.0`에 적용할 수 있습니다.

**PTF SHA-256** `3eab8feb077e0dbb222580542b644e797885eb32aa38470b3901c51301133d36`
