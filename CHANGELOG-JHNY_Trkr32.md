# JHNY_Trkr32 + Sample Editor effects

Unofficial community firmware for **Tracker+ only**, based on CE v1.0.0. Recommended current download; previous releases remain available.

## Downloads / installation
- **TrackerPlus_JHNY_Trkr32.ptf**: copy to `/Firmware/` and install through the normal firmware update menu.
- **EditorFX3.zip**: copy the six `.ptfx` files from its `Effects/` folder into SD `/Effects/`. Replace same-named files if previously installed; retain other effects. Re-enter Sample Editor. Plugin source is included in the ZIP.
- **JHNY_Trkr32.patch**: cumulative firmware source patch against upstream `v1.0.0`. Full firmware and plugin sources are also in this release's Git tag.

## Changes since Trkr26
- Signed Micro Move range **−50 to +50**; live recording chooses the nearest step and records early/late timing. Original OFF behavior is retained; **no automatic Gate insertion**.
- Beat Slice **Split / To slots** creates individual one-shot instruments in empty slots, retaining the original and avoiding occupied slots. Generated samples belong to the project/workspace; this is not a standalone WAV export.
- Instrument+pad audition uses the intended native/C3 reference rather than the current cell's C5 note. Choosing an instrument no longer inserts a note into the cell; releasing Instrument after a selection returns to Note mode.
- Sample Editor raw-sample preview uses C3 consistently, including the Beat Slice/Wavetable preview path.
- Project loading now clears previous instrument/sample/streaming state before loading the next project. This prevents legacy WAV conversion from moving stale instrument slots. Loading displays the stage and instrument slot, e.g. `Convert: sample 06/48` (48 means slot capacity, not loaded instrument count).
- Six offline Sample Editor plugins: **Reverb, Gate, Tape, Transient, Phaser, Tremolo**. Their display names have no JHNY prefix. These process sample audio, not realtime master/Perform sends; Gate here is unrelated to sequencer Gate recording.

Existing Song Repeat editing, MIDI routing/CC recording, opt-in armed tracks, polymeter/track rates, Rotate, Tempo Grid, mixer and pad-guide features are retained.

## Compatibility and verification
C3 remains the MIDI-note-60 display/reference convention used by this build; octave labels vary among MIDI devices. Existing songs are not batch-transposed. Legacy sample instruments retain their stored root/tuning, so their original-pitch note may differ from newly imported C3-root instruments. The Trkr32 load fix introduces no new project/instrument storage format.

The user confirmed that **Arcologies – Ask The Stars now loads on their Tracker+ after installing Trkr32**. This is a specific successful retest, not a complete hardware regression certification. Build, 4,704 host reset/demotion cases, current ARM audition/recording/OFF regression checks, firmware integrity and targeted static-initializer checks passed. Full hardware emulation is incomplete. Plugin DSP and relocated ARM execution checks passed on the preceding binaries; the renamed binaries were verified byte-for-byte identical except for display-name fields. Physical plugin listening/Preview/Apply coverage remains unverified.

---

## 한국어
**Tracker+ 전용 비공식 CE 커뮤니티 펌웨어**입니다. 최신 다운로드는 Trkr32이며 이전 릴리스는 보관합니다.

### 설치
- `TrackerPlus_JHNY_Trkr32.ptf`를 SD `/Firmware/`에 넣고 정상 업데이트 메뉴로 설치합니다.
- `EditorFX3.zip`의 `Effects/` 안 파일 6개를 SD `/Effects/`에 복사합니다. 같은 파일은 덮어쓰고 다른 효과는 유지한 뒤 Sample Editor를 다시 엽니다. ZIP에 플러그인 소스도 포함되어 있습니다.
- `JHNY_Trkr32.patch`는 upstream `v1.0.0` 기준 누적 펌웨어 패치입니다. 릴리스 태그에는 전체 펌웨어·플러그인 소스가 있습니다.

### Trkr26 이후 변경
- 마이크로 무브 **−50~+50**, 라이브 녹음은 가장 가까운 스텝 기준 음수·양수 기록. OFF는 원래 방식 유지, Gate 자동 기록 없음.
- Beat Slice의 **Split / To slots**로 각 슬라이스를 빈 악기 슬롯의 원샷으로 분리. 원본과 기존 악기를 보존하며 결과는 프로젝트/workspace에 포함됩니다. 별도 WAV 내보내기는 아닙니다.
- Instrument+패드 미리듣기 기준을 정리하고 악기 선택으로 셀에 노트가 자동 기록되지 않도록 수정. 선택 후 Instrument를 떼면 Note 모드로 복귀.
- Sample Editor의 원본 샘플 미리듣기를 C3로 통일.
- 프로젝트 교체 시 이전 악기·샘플·스트리밍 상태를 초기화하여 구형 WAV 변환 중 이전 슬롯을 잘못 이동시키는 경로 수정. 로딩 단계와 악기 슬롯 번호 표시.
- **Reverb / Gate / Tape / Transient / Phaser / Tremolo** 샘플 에디터 이펙트 추가. 표시 이름에서 JHNY 제거. 샘플 파형을 가공하는 오프라인 효과이며 마스터/퍼폼 실시간 센드와는 별개입니다.

기존 Song Repeat, MIDI 라우팅·CC 녹음, 기본 언암드, 폴리미터·속도, Rotate, Tempo Grid, 믹서·패드 가이드는 유지합니다.

### 호환성·확인 범위
C3를 MIDI 60과 맞춘 기존 기준을 유지합니다. MIDI 장비마다 옥타브 이름은 다를 수 있습니다. 기존 곡을 일괄 이조하지 않으며 구형 샘플 악기의 루트·튜닝은 보존하므로 새 C3 기준 악기와 원음 노트가 다를 수 있습니다. 이번 로딩 수정으로 저장 형식을 바꾸지 않았습니다.
사용자가 Trkr32 설치 후 **Arcologies – Ask The Stars 로드 성공을 확인**했습니다. 모든 기기·프로젝트 검증을 의미하지는 않습니다. 빌드와 자동 회귀검사·파일 무결성 검사를 통과했으며, 이펙트 이름 변경은 이름 필드 외 모든 바이너리 바이트가 동일함을 확인했습니다. 이펙트의 실기 청감·Preview/Apply 전체 검증은 아직 하지 않았습니다.
