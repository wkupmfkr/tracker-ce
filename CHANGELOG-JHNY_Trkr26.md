# JHNY_Trkr26 — Song Repeat editing

Tracker+ only. Based on the user-validated JHNY_Trkr25.

## 한국어

### 변경 사항
- Song의 좌측 트랙 블록과 우측 패턴 목록이 동일한 반복 묶음과 스크롤 기준으로 표시됩니다.
- 연속된 같은 패턴은 한 행으로 표시합니다. 예: P2, P3, P1, P1, P3, P4 → P2 / P3 / P1 ×2 / P3 / P4.
- REC에서도 묶음을 유지하며 선택 테두리, 재생 위치 표시도 해당 묶음 행을 따릅니다.
- 하단에 Pattern / Repeat 버튼을 각각 제공합니다.
- Pattern 편집은 선택한 묶음 전체의 패턴 번호를 바꿉니다. 인접한 같은 번호의 묶음은 합쳐집니다.
- Add Slot은 선택한 묶음 뒤에 다음 패턴 번호의 행을 추가합니다. Delete Slot은 선택한 묶음을 삭제합니다. 유일하게 남은 묶음은 삭제하지 않습니다.
- Repeat / Pattern / Add Slot / Delete Slot의 마지막 송 목록 편집은 Undo로 되돌릴 수 있습니다.
- 트랙 내용 복사·붙여넣기·삭제도 보이는 묶음 행 단위를 따릅니다. 반복된 한 행을 복사했다고 다음 행까지 덮어쓰지 않습니다.
- 총 255슬롯 한계를 지키며, 마지막 슬롯에서의 다음 패턴 조회와 송 순환 시 배열 경계를 보완했습니다.

### 사용법
1. 재생을 멈추고 Song 화면에서 REC를 켭니다.
2. 편집할 행을 선택합니다.
3. Repeat 탭: 반복 횟수 +1. Shift + Repeat 탭: −1.
4. Repeat를 누른 채 노브 또는 방향키: 횟수 조절. 위/오른쪽은 증가, 아래/왼쪽은 감소.
5. Pattern도 동일한 조작으로 묶음 전체의 패턴 번호를 조절합니다.
6. Undo는 마지막 송 목록 편집 1회를 되돌립니다.

반복 횟수는 최소 1이며 전체 송은 255슬롯까지입니다. 예를 들어 P1 ×8은 저장 시 기존 방식의 P1 슬롯 8개입니다. 송 구조 편집과 해당 Undo는 재생 정지 상태에서만 가능합니다. 재생 중 시도하면 정지 안내를 표시합니다. 이는 다음 패턴 미리 읽기와 편집의 충돌을 피하기 위한 제한입니다.

### 호환성과 유지 사항
저장 형식에 새로운 Repeat 필드를 추가하지 않았습니다. 기존 송을 그대로 묶어 보여주고, 반복 횟수 편집은 기존 슬롯 배열에 반영됩니다. 패턴의 노트·음높이·악기 데이터를 이 변경으로 변환하지 않습니다. Trkr25의 C3 처리, MIDI 라우팅/녹음, 기본 언암드, 폴리미터/속도, 믹서, 루트 가이드, Rotate, Tempo Grid는 변경하지 않았습니다.
송 목록의 구조를 바꾸면 이전 위치를 가리키던 Song 복사 선택은 해제되므로 다시 Copy해 주세요.

### 검증과 한계
ARM 빌드, 실제 변경 함수 기반 호스트 검사, 10,000개 무작위 슬롯 배열 검사, 255슬롯 경계/순환, 묶음 편집·Undo·복사·삭제·렌더링 검사를 통과했습니다. 새 ARM 실행 파일에서 16트랙 기본 언암드/개별 암드도 확인했습니다.
SDRAM을 매핑하지 않은 ARM 검사에서 패턴 및 Song 에디터 정적 초기화가 완료되었습니다. 전체 초기화 검사는 하드웨어 모델 한계로 116개 중 13개 이후 완료되지 않았으며, 실제 부팅 보장은 아닙니다.
실제 Tracker+ 설치·화면·오디오 검증은 아직 하지 않았습니다. 기존 Trkr25 파일은 보존했습니다.

## English

### Changes
- Both sides of Song view now share the same consecutive-pattern groups and scroll origin, including REC mode.
- Track blocks, selection frames and the playback line align with the grouped pattern rows.
- Separate Pattern and Repeat controls edit the entire selected group.
- Adjacent identical patterns merge automatically.
- Add Slot inserts the next pattern number after the selected group; Delete Slot removes the group while retaining a nonempty song.
- One-level Undo restores the last playlist edit.
- Track-content copy/paste/delete follows visible groups, so copying one repeated row does not overwrite later rows.
- Enforces the 255-slot capacity and fixes next-pattern lookup/wrap bounds at the final slot.

### Controls
Stop playback, open Song and enable REC. Select a row.
- Tap Repeat: +1; Shift + tap: -1.
- Hold Repeat and turn the jog wheel or use arrows to adjust the count.
- Pattern uses the same gestures to change the whole group's pattern number.
- Undo restores the last playlist edit.

Structural playlist edits and their Undo require stopped playback to avoid conflicting with next-pattern preloading. Repeats remain ordinary playlist slots; no new serialized repeat field is introduced. Existing song playback order and pattern note data are retained. All non-Song Trkr25 features remain unchanged, apart from the related full-capacity Song bounds fixes. Structural playlist edits clear the previous Song copy selection.

### Validation
Firmware build, actual-function host tests, 10,000 randomized slot tests, grouped rendering/editing/copy/delete/undo and full-capacity playback wrap passed. Actual ARM tests confirmed default unarmed states and arm toggles.
Targeted pre-SDRAM pattern and Song editor initializer checks passed. The full initializer emulation remains incomplete (13/116; hardware-model limitation), and is not a hardware boot test.
Physical installation, display and audio tests have NOT been run.

Reference: [simone.iannicelli's Song repeats feature](https://backstage.polyend.com/t/polymeter-rotate-chance-conditions-per-track-midi-in/25145) and its Tracker-CE-Mods manual. This implementation uses the existing slot storage model.

## Files
- TrackerPlus_JHNY_Trkr26.ptf — installation firmware
- JHNY_Trkr26.patch — cumulative source patch against upstream v1.0.0
- validation.json — build and validation evidence
- TEST-RESULTS.txt — host test results
- SHA256SUMS.txt — checksums
