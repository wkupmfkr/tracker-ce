#include "core/interfaceMenuPopup.h"
#include "fileManager.h"
#include "songEditor/songEditor.h"

#include "core/interfacePopups.h"
#include "padToBeatNoteConverter.h"
#include "patternEditor/patternEditor.h" // todo zrobic cos z tym
#include "performanceMode/performanceMode.h"

#include "Platform.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"
#include "mtStructs.h"
#include "util_math.h"
#include "songRepeats.h"

cSongEditor songEditor;

static cSongEditor* SE = &songEditor;

static void functPlayPattern();
static void functPlaySong();
static void functIncPattern(uint8_t state);
static void functDecPattern(uint8_t state);
static void functDeleteSlot();
static void functAddSlot();

static void functTempo(uint8_t state);

static void functUp();
static void functDown();
static void functRight();
static void functLeft();

static void functShift(uint8_t state);
static void functInsert();

static void functPlayAction();
static void functRecAction();

static void functEncoder(int16_t value);

static void functSwitchModule(uint8_t button);
static void functSwitchModeSong(uint8_t state);

static void functPads(uint8_t pad, uint8_t state, int16_t velo);

static void functMenu(uint8_t state);

static void functCopyPaste();
static void refreshCopyPasting();
static void updateBitmaskAfterCopy(uint16_t* srcBitmask,
                                   uint16_t* destBitmask,
                                   uint8_t startSrc,
                                   uint8_t startDest,
                                   uint8_t length);

static void functCopy();
static void functPaste();

static void functUndo();

static void functDelete();
static void refreshDeleting();

void playSongFromSelectedPattern();
void playSongFromSelectedPattern_stop();

void cSongEditor::update()
{
    if (songPlayerRefreshTimer > 100)
    {
        songPlayerRefreshTimer = 0;
        refreshButtonsLabels();

        Sequencer::enSeqState seqState = sequencer.getSeqState();
        if (seqState == Sequencer::SEQ_STATE_PLAY_SONG)
        {
            if (lastSongListPos != mtProject.song.playlistPos ||
                selectedSongPosition != mtProject.song.playlistPos)
            {
                lastSongListPos = mtProject.song.playlistPos;
                if (!editMode)
                {
                    selectedSongPosition = mtProject.song.playlistPos;
                    refreshPatternsList();
                    display.setControlValue(songPlayerControl, selectedSongPosition);
                    display.refreshControl(songPlayerControl);
                }
                showIcon(iconPlay, lastSongListPos);
            }
        }
        else if (seqState == Sequencer::SEQ_STATE_PLAY_SELECTION)
        {
            if (lastSongListPos != selectedSongPosition)
            {
                lastSongListPos = selectedSongPosition;
                display.setControlValue(songPlayerControl, selectedSongPosition);
                display.refreshControl(songPlayerControl);
            }
        }
        else if (seqState == Sequencer::SEQ_STATE_PLAY_PATTERN ||
                 seqState == Sequencer::SEQ_STATE_PLAY_PERFORMANCE)
        {
            if (sequencer.isPlay() && !editMode)
            {
                lastSongListPos = mtProject.song.playlistPos;
            }

            if (lastSongListPos != selectedSongPosition)
            {
                if (!editMode)
                {
                    selectedSongPosition = lastSongListPos;
                    display.setControlValue(songPlayerControl, selectedSongPosition);
                    display.refreshControl(songPlayerControl);
                }

                icon_t playerIcon =
                  patternsList.icon.iconNum == displayPlayIcon + iconLoop ? iconLoop : iconPlay;
                showIcon(playerIcon, lastSongListPos);
            }
        }
        if (seqState == Sequencer::SEQ_STATE_STOP)
        {
            hideIcon();
            songPlayerData.progress.isPlaying = 0;
        }
        else
        {
            songPlayerData.progress.isPlaying = 1;
        }
        refreshSongPlayer(lastSongListPos);
        refreshTempoField();
    }

    tapTempo_update();
    refreshCopyPasting();
    refreshDeleting();
}

void cSongEditor::start(uint32_t options)
{
    moduleRefresh = 1;
    lastSongListPos = -1;
    isSomethingCopied = 0;

    repeatControl = 0;
    repeatControlUsed = false;
    playlistUndoValid = false;
    pressedSongButton = false;
    exitOnButtonRelease = 0;

    newFileManager.updateActualPatternBitmask();
    readSong();
    handleEntryIcon();

    // ustawienie funkcji
    ip->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonSong, functSwitchModeSong);
    ip->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);
    ip->setButtonObj(interfaceButtonGame, buttonPress, functSwitchModule);

    ip->setButtonObj(interfaceButtonMiniMenu, functMenu);

    refreshEditMode();
    showDefaultScreen();
    setDefaultScreenFunct();

    lightUpPadBoard();
}

void cSongEditor::stop()
{
    sequencer.stopManualNotes();
    cleanPatternsListResources();

    moduleRefresh = 0;
}

void cSongEditor::setDefaultScreenFunct()
{
    // funkcje
    ip->setPotObj(interfacePot0, functEncoder, false);

    ip->clearButtonsRange(interfaceButton0, interfaceButton7);

    ip->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
    ip->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

    ip->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
    ip->setButtonObj(interfaceButtonRight, buttonPress, functRight);
    ip->setButtonObj(interfaceButtonUp, buttonPress, functUp);
    ip->setButtonObj(interfaceButtonDown, buttonPress, functDown);

    ip->setButtonObj(interfaceButtonInsert, buttonPress, functInsert);

    ip->setButtonObj(interfaceButtonShift, functShift);

    ip->setButtonObj(interfaceButton0, buttonPress, functPlayPattern);
    ip->setButtonObj(interfaceButton1, buttonPress, functPlaySong);
    ip->setButtonObj(interfaceButton2, buttonPress, functUndo);
    ip->setButtonObj(interfaceButton3, buttonPress, functAddSlot);
    ip->setButtonObj(interfaceButton4, buttonPress, functDeleteSlot);
    ip->setButtonObj(interfaceButton5, functDecPattern);
    ip->setButtonObj(interfaceButton6, functIncPattern);
    ip->setButtonObj(interfaceButton7, functTempo);

    ip->setButtonObj(interfaceButtonCopy, buttonPress, functCopyPaste);
    ip->setButtonObj(interfaceButtonDelete, buttonPress, functDelete);

    ip->setPadsGlobal(functPads);
}

void cSongEditor::lightUpPadBoard()
{
    padsBacklight.clearGlobalPads(0, 1, 1);

    bool isBeatSlice =
      mtProject.values.lastUsedInstrument < kSampleInstrumentsCount &&
      mtProject.instrument[mtProject.values.lastUsedInstrument].playMode == playModeBeatSlice;

    if (isBeatSlice)
    {
        for (uint8_t pad = 0;
             pad < interfacePadsCount && pad < mtProject.instrument[mtProject.values.lastUsedInstrument].sliceNumber;
             pad++)
        {
            padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, pad);
        }
    }
}

//==============================================================================================================

static void functPlayPattern()
{
    Sequencer::enSeqState seqState = sequencer.getSeqState();

    if (seqState == Sequencer::SEQ_STATE_STOP)
    {
        newFileManager.setSongPos(SE->selectedSongPosition);
        if (SE->buttons->isButtonPressed(interfaceButtonShift) && SE->selectedPlace == 0 &&
            SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) ==
            SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition + SE->songPlayerData.selection.patternSelectionLength - 1) &&
            SE->songPlayerData.selection.trackSelectionLength == 1) // xxx
        {
            // tylko zaznaczony track
            SE->switchToNewPattern();

            sequencer.setSelection(0,
                                   SE->songPlayerData.selection.startTrack,
                                   sequencer.getPatternLength() - 1,
                                   SE->songPlayerData.selection.startTrack);

            sequencer.playSelection();

            SE->startSongPlayer();
            // wlacza tryb preview tracku \/
            SE->songPlayerData.progress.trackPreview = SE->songPlayerData.selection.startTrack;
            SE->lastSongListPos = SE->selectedSongPosition;

            display.refreshControl(SE->songPlayerControl);
            return;
        }

        // caly pattern
        SE->switchToNewPattern();

        sequencer.playPattern();
        SE->lastSongListPos = SE->selectedSongPosition;
        SE->showIcon(iconLoop, SE->selectedSongPosition);

        SE->startSongPlayer();

        display.refreshControl(SE->songPlayerControl);
    }
    else if (seqState == Sequencer::SEQ_STATE_PLAY_SELECTION)
    {
        playSongFromSelectedPattern_stop();
    }
    else
    {
        sequencer.setOnGlobalStep(cbkSongPlaybackModeToggle);
    }
}

/* Executed from the sequencer interrupt. */
void cbkSongPlaybackModeToggle()
{
    Sequencer::enSeqState seqState = sequencer.getSeqState();
    uint8_t selectedSlot = mtProject.song.playlistPos;

    if (newFileManager.isPatternLoading() || sequencer.isLastPatternStep())
    {
        return; // retry in one step (because we're not unsetting)
    }

    if (seqState == Sequencer::SEQ_STATE_PLAY_SONG)
    {
        sequencer.loopCurrentPattern(selectedSlot);
        SE->showIcon(iconLoop, selectedSlot);
    }
    else if (seqState == Sequencer::SEQ_STATE_PLAY_PERFORMANCE)
    {
        if (sequencer.isSongMode())
        {
            // już ktoś wcisnął "Continue Song" i teraz zmienił zdanie
            sequencer.loopCurrentPattern(selectedSlot);
            sequencer.setOnPatternOrSelectionEnd(nullCallback);
            SE->showIcon(iconLoop, selectedSlot);
        }
        else
        {
            sequencer.continueSong(selectedSlot);
            performanceMode.turnOffAtEndOfPattern();
            SE->showIcon(iconPlay, selectedSlot);
        }
    }
    else if (seqState == Sequencer::SEQ_STATE_PLAY_PATTERN)
    {
        sequencer.continueSong(selectedSlot);
        SE->showIcon(iconPlay, selectedSlot);
    }

    sequencer.setOnGlobalStep(nullCallback);
}

void playSongFromSelectedPattern()
{
    sequencer.playSongFromPosition(SE->selectedSongPosition);
    SE->showIcon(iconPlay, SE->selectedSongPosition);

    SE->startSongPlayer();
}

void playSongFromSelectedPattern_stop()
{
    SE->lastSongListPos = -1;
    sequencer.stop();
    SE->hideIcon();
    if (performanceMode.turnOff())
    {
        newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
        sequencer.switchRamPatternsNow();
    }
}

static void functPlaySong()
{
    if (sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
    {
        playSongFromSelectedPattern();
    }
    else if (sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
    {
        playSongFromSelectedPattern_stop();
    }
}

static void functIncPattern(uint8_t state)
{
    if (SE->isBusy || SE->exitOnButtonRelease || !SE->editMode) return;
    if (state == buttonPress) {
        SE->repeatControl = 6;
        SE->repeatControlUsed = false;
        SE->selectedPlace = 1;
        SE->activateLabelsBorder();
    } else if (state == buttonRelease && SE->repeatControl == 6) {
        const bool tap = !SE->repeatControlUsed;
        SE->repeatControl = 0;
        if (tap) {
            int delta = SE->buttons->isButtonPressed(interfaceButtonShift) ? -1 : 1;
            SE->changeRepeatCount(delta);
        }
    }
}

static void functDecPattern(uint8_t state)
{
    if (SE->isBusy || SE->exitOnButtonRelease || !SE->editMode) return;
    if (state == buttonPress) {
        SE->repeatControl = 5;
        SE->repeatControlUsed = false;
        SE->selectedPlace = 1;
        SE->activateLabelsBorder();
    } else if (state == buttonRelease && SE->repeatControl == 5) {
        const bool tap = !SE->repeatControlUsed;
        SE->repeatControl = 0;
        if (tap) {
            int delta = SE->buttons->isButtonPressed(interfaceButtonShift) ? -1 : 1;
            SE->changeSelectedPlayListPattern(delta);
        }
    }
}

static void functAddSlot()
{
    if (SE->isBusy || SE->exitOnButtonRelease || !SE->editMode || SE->songLength >= SONG_MAX) return;
    if (!sequencer.isStop()) { mtPopups.showInfoPopup("Stop playback", "to add Song row"); return; }
    const int after = SongRepeats::end(mtProject.song.playlist, SE->songLength, SE->selectedSongPosition);
    int pattern = mtProject.song.playlist[SE->selectedSongPosition] % PATTERN_INDEX_MAX + 1;
    if (SE->buttons->isButtonPressed(interfaceButtonShift)) {
        pattern = 0;
        for (int i = 0; i < PATTERN_INDEX_MAX; ++i) {
            if (!mtProject.values.allPatternsBitmask[i] && !mtProject.values.allPatternsBitmaskMidi[i]) {
                pattern = i + 1; break;
            }
        }
        if (!pattern) return;
    }
    SE->replaceSongRange(after, after, 1, pattern);
    SE->selectedSongPosition = after;
    SE->selectedPlace = 1;
    SE->refreshSongPlayerSelectedSlot();
    SE->activateLabelsBorder();
}

static void functDeleteSlot()
{
    if (SE->isBusy || SE->exitOnButtonRelease || !SE->editMode) return;
    const int first = SongRepeats::begin(mtProject.song.playlist, SE->songLength, SE->selectedSongPosition);
    const int last = SongRepeats::end(mtProject.song.playlist, SE->songLength, first);
    if (last - first == SE->songLength) return; // keep a nonempty song
    SE->replaceSongRange(first, last, 0, 1);
    SE->selectedPlace = 1;
    SE->activateLabelsBorder();
}

void cSongEditor::tapTempo_tap()
{
    tapTempoActive = 1;

    if (SE->tapTempoCount == 0)
    {
        SE->tapTempoCount++;
        SE->tapTempoTimer = 0;
        SE->refreshTempoField();
    }
    else
    {
        SE->tapTempoCount++;

        if (!tapTempo_still_going())
        {
            tapTempo_end();
        }
        else if (SE->tapTempoCount >= 4)
        {
            float tempTempo = 60000.0f * 3.0f / (float)tapTempoTimer;

            tapTempo_set(tempTempo);

            SE->tapTempoTimer = 0;
            SE->tapTempoCount = 1;

            tapTempo_end();
        }
        SE->refreshTempoField();
    }
}

bool cSongEditor::tapTempo_still_going()
{
    return tapTempoCount > 0 && (tapTempoTimer < (1000UL * (tapTempoCount + 1)));
}

void cSongEditor::tapTempo_update()
{
    if (tapTempoActive && !tapTempo_still_going())
    {
        tapTempo_end();
    }
}

void cSongEditor::tapTempo_set(float value)
{
    if (value > Sequencer::MAX_TEMPO)
    {
        return;
    }
    if (value < Sequencer::MIN_TEMPO)
    {
        return;
    }

    if (sequencer.isSetInternalClock())
    {
        mtProject.values.globalTempo = value;
    }
}

void cSongEditor::tapTempo_end()
{
    tapTempoCount = 0;
    tapTempoActive = 0;
    refreshTempoField();
}

static void functTempo(uint8_t state)
{
    if (SE->isBusy)
    {
        return;
    }

    if (state == buttonPress)
    {
        if (SE->selectedPlace == 2)
        {
            SE->tapTempo_tap();
            return;
        }
        else
        {
            SE->selectedPlace = 2;
        }
    }
    else if (state == buttonRelease)
    {
    }
    else
    {
        return;
    }

    SE->activateLabelsBorder();
}

//==============================================================================================================

static void functEncoder(int16_t value)
{
    if (SE->repeatControl && SE->editMode && !SE->isBusy) {
        SE->repeatControlUsed = true;
        if (SE->repeatControl == 6) SE->changeRepeatCount(value);
        else SE->changeSelectedPlayListPattern(value);
        return;
    }

    if (SE->isBusy)
    {
        return;
    }

    switch (SE->selectedPlace)
    {
        case 1:
        {
            if (SE->buttons->isButtonPressed(interfaceButtonShift))
            {
                SE->changeSelectedPlayListPattern(value);
            }
            else
            {
                SE->changeSongPositionSelection(interfaceGlobals.invertValForList ? -value : value);
            }
            break;
        }
        case 2:
        {
            if (SE->buttons->isButtonPressed(interfaceButtonShift))
            {
                SE->changeGlobalTempo(0.1 * float(value));
            }
            else
            {
                SE->changeGlobalTempo(1 * float(value));
            }
            break;
        }
        default: break;
    }
}

static void functShift(uint8_t state)
{
    if (state == buttonPress)
    {
        SE->refreshAddSlotLabel(true);
    }
    else if (state == buttonRelease)
    {
        SE->refreshAddSlotLabel(false);
    }
}

static void functUp()
{
    if (SE->repeatControl && SE->editMode && !SE->isBusy) {
        SE->repeatControlUsed = true;
        if (SE->repeatControl == 6) SE->changeRepeatCount(1);
        else SE->changeSelectedPlayListPattern(1);
        return;
    }

    if (SE->isBusy)
    {
        return;
    }

    switch (SE->selectedPlace)
    {
        case 0: SE->walkOnSongPlayer(playerUp); break;
        case 1: SE->changeSongPositionSelection(-1); break;
        case 2:
            if (SE->buttons->isButtonPressed(interfaceButtonShift))
            {
                SE->changeGlobalTempo(0.1);
            }
            else
            {
                SE->changeGlobalTempo(1);
            }
            break;
    }
}

static void functDown()
{
    if (SE->repeatControl && SE->editMode && !SE->isBusy) {
        SE->repeatControlUsed = true;
        if (SE->repeatControl == 6) SE->changeRepeatCount(-1);
        else SE->changeSelectedPlayListPattern(-1);
        return;
    }

    if (SE->isBusy)
    {
        return;
    }

    switch (SE->selectedPlace)
    {
        case 0: SE->walkOnSongPlayer(playerDown); break;
        case 1: SE->changeSongPositionSelection(1); break;
        case 2:
        {
            if (SE->buttons->isButtonPressed(interfaceButtonShift))
            {
                SE->changeGlobalTempo(-0.1);
            }
            else
            {
                SE->changeGlobalTempo(-1);
            }
            break;
        }
    }
}

static void functLeft()
{
    if (SE->repeatControl && SE->editMode && !SE->isBusy) {
        SE->repeatControlUsed = true;
        if (SE->repeatControl == 6) SE->changeRepeatCount(-1);
        else SE->changeSelectedPlayListPattern(-1);
        return;
    }

    if (SE->isBusy)
    {
        return;
    }
    if (SE->exitOnButtonRelease)
    {
        return;
    }
    if (!SE->editMode)
    {
        if (SE->selectedPlace == 2)
        {
            SE->selectedPlace = 1;
            SE->activateLabelsBorder();
        }
        return;
    }

    if (SE->selectedPlace == 0)
    {
        SE->walkOnSongPlayer(playerLeft);
    }
    else if (SE->selectedPlace == 1)
    {
        if (SE->songPlayerData.selection.isActive != 1)
        {
            const int allTracksCount =
              SE->songPlayerData.normalTracksCount + SE->songPlayerData.midiTracksCount;
            SE->selectSpecificTrack(allTracksCount - 1, SE->selectedSongPosition);
            SE->selectedPlace = 0;
        }
    }
    else if (SE->selectedPlace == 2)
    {
        SE->selectedPlace = 1;
    }

    SE->activateLabelsBorder();
}

static void functRight()
{
    if (SE->repeatControl && SE->editMode && !SE->isBusy) {
        SE->repeatControlUsed = true;
        if (SE->repeatControl == 6) SE->changeRepeatCount(1);
        else SE->changeSelectedPlayListPattern(1);
        return;
    }

    if (SE->isBusy)
    {
        return;
    }
    if (SE->exitOnButtonRelease)
    {
        return;
    }
    if (!SE->editMode)
    {
        if (SE->selectedPlace == 1)
        {
            SE->selectedPlace = 2;
            SE->activateLabelsBorder();
        }
        return;
    }

    const int allTracksCount =
      SE->songPlayerData.normalTracksCount + SE->songPlayerData.midiTracksCount;

    if (SE->selectedPlace == 0 && SE->songPlayerData.selection.startTrack < allTracksCount - 1)
    {
        SE->walkOnSongPlayer(playerRight);
    }
    else if (SE->selectedPlace == 0)
    {
        SE->selectedPlace = 1;
    }
    else if (SE->selectedPlace == 1)
    {
        SE->selectedPlace = 2;
    }

    SE->activateLabelsBorder();
}

static void functPlayAction()
{
    if (SE->isBusy)
    {
        return;
    }

    if (SE->buttons->isButtonPressed(interfaceButtonShift))
    {
        functPlayPattern();
    }
    else
    {
        functPlaySong();

        if (SE->buttons->isButtonPressed(interfaceButtonSong))
        {
            SE->exitOnButtonRelease = 1;
        }
    }
}

static void functRecAction()
{
    if (SE->isBusy)
    {
        return;
    }

    SE->editMode = !SE->editMode;

    SE->refreshEditMode();
}

static void functSwitchModule(uint8_t button)
{
    if (SE->isBusy)
    {
        return;
    }

    SE->eventFunct(eventSwitchModule, SE, &button, 0);
}

//======================================================================================================================
void cSongEditor::startSongPlayer()
{
    songPlayerData.progress.isPlaying = 1;
    songPlayerData.progress.trackPreview = 255; // wylacza tryb preview tracku
    songPlayerData.progress.currentSongPosition = selectedSongPosition;
    songPlayerData.progress.patternLength = sequencer.getPatternLength();
    songPlayerData.progress.positionInPattern = sequencer.getActualPos();

    display.refreshControl(songPlayerControl);
}

void cSongEditor::stopSongPlayer()
{
    songPlayerData.progress.isPlaying = 0;

    display.refreshControl(songPlayerControl);
}

void cSongEditor::stopPlayingSelection()
{
    if (sequencer.ptrPlayer->selectionMode)
    {
        sequencer.stop();
        display.refreshControl(songPlayerControl);
    }
}

void cSongEditor::refreshSongPlayer(uint8_t source)
{
    if (sequencer.isStop())
    {
        display.refreshControl(songPlayerControl);
        return;
    }

    songPlayerData.progress.currentSongPosition = source;
    songPlayerData.progress.patternLength = sequencer.getPatternLength();
    songPlayerData.progress.positionInPattern = sequencer.getActualPos();

    display.refreshControl(songPlayerControl);
}

void cSongEditor::changeSongPositionSelection(int16_t value)
{
    if (!editMode && !sequencer.isStop()) return;
    selectedSongPosition = SongRepeats::move(mtProject.song.playlist, songLength, selectedSongPosition, value);
    refreshPatternsList();
    refreshSongPlayerSelectedSlot();
}

//======================================================================================================================
//==============================================================================================
void cSongEditor::refreshEditMode()
{
    repeatControl = 0;
    for (uint8_t i = 2; i < 7; i++)
    {
        display.setControlColors(label[i],
                                 editMode ? display.getTheme().activeButtonLabelsColors
                                          : display.getTheme().inactiveButtonLabelsColors);
        display.refreshControl(label[i]);
    }

    if (editMode)
    {
    }
    else
    {
        if (selectedPlace == 0)
        {
            selectedPlace = 1;
        }
    }

    refreshPatternsList();
    activateLabelsBorder();

    display.refreshControl(songPlayerControl);
    display.synchronizeRefresh();
}

//======================================================================================================================
//==============================================================================================

void cSongEditor::readSong()
{
    uint8_t i;
    for (i = 0; i < SONG_MAX; i++)
    {
        if (mtProject.song.playlist[i] == 0)
        {
            break;
        }
    }

    songLength = i;

    if (songLength == 0) // nie znaleziono
    {
        songLength = 1;
        selectedSongPosition = 0;

        mtProject.song.playlist[0] = 1;
    }

    songPlayerData.songLength = songLength;

    refreshPatternsList();

    if (selectedSongPosition >= songLength)
    {
        selectedSongPosition = 0;
    }
    patternsList.start = selectedSongPosition;

    songPlayerData.normalTracksCount = kSampleTracks;
    songPlayerData.midiTracksCount = kMidiTracks;
    songPlayerData.normalTracksNames = &mtProject.values.TrackNames[0][0];
    songPlayerData.midiTracksNames = &mtProject.values.midiTrackNames[0][0];
    songPlayerData.normalTracksNamesBuffLen = 21;
    songPlayerData.midiTracksNamesBuffLen = TRACK_NAME_LENGTH + 1;
    songPlayerData.list = &patternsList;
    songPlayerData.songData = mtProject.song.playlist;
    songPlayerData.normalPatternsBitmask = mtProject.values.allPatternsBitmask;
    songPlayerData.midiPatternsBitmask = mtProject.values.allPatternsBitmaskMidi;
    songPlayerData.normalTracksMute = mtProject.values.trackMute;
    songPlayerData.midiTracksMute = mtProject.values.midiTrackMute;
    songPlayerData.isListActive = 1;
    songPlayerData.isEditMode = &editMode;

    display.setControlData(songPlayerControl, &songPlayerData);
}

void cSongEditor::refreshPatternsList()
{
    patternsList.length = songLength;
    patternsList.linesCount = 12;
    patternsList.data = mtProject.song.playlist;

    if (patternNames != nullptr)
    {
        delete[] patternNames;
    }

    patternNames = new char*[songLength];

    for (int slot = 0; slot < songLength; slot++)
    {
        char* patternName =
          newFileManager.getPatternsMetadata()->getPatternName(mtProject.song.playlist[slot]);

        if (patternName == nullptr || patternName[0] == 0)
        {
            patternNames[slot] = (char*)kEmptyPatternNameText;
        }
        else
        {
            patternNames[slot] = patternName;
        }
    }

    patternsList.names = patternNames;
}

void cSongEditor::cleanPatternsListResources()
{
    if (patternNames != nullptr)
    {
        delete[] patternNames;
        patternNames = nullptr;
    }
}

void cSongEditor::changeGlobalTempo(float value)
{
    //	nie zmieniaj jak external clock
    if (!sequencer.isSetInternalClock())
    {
        return;
    }

    tapTempo_end(); // jak zmiana to wylacz TAP

    if (remainder(value, 1.0) == 0.0)
    {
        mtProject.values.globalTempo = (uint16_t)mtProject.values.globalTempo; // do calkowitych
    }

    if (mtProject.values.globalTempo + value < Sequencer::MIN_TEMPO)
        mtProject.values.globalTempo = Sequencer::MIN_TEMPO;
    else if (mtProject.values.globalTempo + value > Sequencer::MAX_TEMPO)
        mtProject.values.globalTempo = Sequencer::MAX_TEMPO;
    else
        mtProject.values.globalTempo += value;

    newFileManager.setProjectStructChanged();

    showTempoValue();
}

void cSongEditor::setDefaultGlobalTempo()
{
    //	nie zmieniaj jak external clock
    if (!sequencer.isSetInternalClock())
    {
        return;
    }

    mtProject.values.globalTempo = DEFAULT_TEMPO;
    // TODO: podpiąć afektowanie aktulanego odtwarzania

    showTempoValue();
}

void cSongEditor::changeSelectedPlayListPattern(int value)
{
    if (!editMode || selectedPlace != 1 || !value) return;
    const int first = SongRepeats::begin(mtProject.song.playlist, songLength, selectedSongPosition);
    const int last = SongRepeats::end(mtProject.song.playlist, songLength, first);
    const int pattern = clamp<int>(mtProject.song.playlist[first] + value, 1, PATTERN_INDEX_MAX);
    if (pattern != mtProject.song.playlist[first]) replaceSongRange(first, last, last - first, pattern);
}

void cSongEditor::changeRepeatCount(int delta)
{
    if (!editMode || !delta) return;
    const int first = SongRepeats::begin(mtProject.song.playlist, songLength, selectedSongPosition);
    const int last = SongRepeats::end(mtProject.song.playlist, songLength, first);
    const int copies = clamp<int>(last - first + delta, 1, SONG_MAX - songLength + last - first);
    if (copies != last - first) replaceSongRange(first, last, copies, mtProject.song.playlist[first]);
}

void cSongEditor::replaceSongRange(int first, int last, int copies, uint8_t pattern)
{
    if (!sequencer.isStop()) {
        mtPopups.showInfoPopup("Stop playback", "to edit Song repeats");
        return;
    }
    // Prepare outside the critical section; publish all slots and cursor atomically.
    uint8_t next[SONG_MAX];
    memcpy(next, mtProject.song.playlist, SONG_MAX);
    const int newLength = SongRepeats::replace(next, songLength, SONG_MAX, first, last, copies, pattern);
    memcpy(playlistUndo, mtProject.song.playlist, SONG_MAX);
    playlistUndoLength = songLength;
    playlistUndoSelection = selectedSongPosition;
    uint32_t irq;
    asm volatile("mrs %0, primask\n cpsid i" : "=r"(irq) :: "memory");
    playlistUndoPlayPosition = mtProject.song.playlistPos;
    const int playing = SongRepeats::remap(mtProject.song.playlistPos, first, last, copies, newLength);
    memcpy(mtProject.song.playlist, next, SONG_MAX);
    mtProject.song.playlistPos = playing;
    asm volatile("msr primask, %0" :: "r"(irq) : "memory");
    playlistUndoValid = true;
    isSomethingCopied = 0;
    selectedSongPosition = SongRepeats::begin(next, newLength,
        SongRepeats::remap(selectedSongPosition, first, last, copies, newLength));
    lastSongListPos = playing;
    songLength = newLength;
    songPlayerData.songLength = songLength;
    songPlayerData.selection.isActive = 0;
    songPlayerData.selection.patternSelectionLength = 1;
    songPlayerData.selection.startSongPosition = selectedSongPosition;
    songPlayerData.progress.currentSongPosition = playing;
    patternsList.icon.iconPositionInList = playing;
    refreshPatternsList();
    refreshSongPlayerSelectedSlot();
    newFileManager.setProjectStructChanged();
}

bool cSongEditor::undoPlaylistEdit()
{
    if (!playlistUndoValid) return false;
    if (!sequencer.isStop()) { mtPopups.showInfoPopup("Stop playback", "to undo Song edit"); return true; }
    uint32_t irq;
    asm volatile("mrs %0, primask\n cpsid i" : "=r"(irq) :: "memory");
    memcpy(mtProject.song.playlist, playlistUndo, SONG_MAX);
    mtProject.song.playlistPos = playlistUndoPlayPosition;
    asm volatile("msr primask, %0" :: "r"(irq) : "memory");
    songLength = playlistUndoLength;
    selectedSongPosition = playlistUndoSelection;
    lastSongListPos = playlistUndoPlayPosition;
    playlistUndoValid = false;
    isSomethingCopied = 0;
    songPlayerData.songLength = songLength;
    songPlayerData.selection.isActive = 0;
    songPlayerData.progress.currentSongPosition = lastSongListPos;
    patternsList.icon.iconPositionInList = lastSongListPos;
    refreshPatternsList();
    refreshSongPlayerSelectedSlot();
    newFileManager.setProjectStructChanged();
    return true;
}

static void functPads(uint8_t pad, uint8_t state, int16_t)
{
    if (SE->isBusy)
    {
        return;
    }

    bool isBeatSlice =
      mtProject.values.lastUsedInstrument < kSampleInstrumentsCount &&
      mtProject.instrument[mtProject.values.lastUsedInstrument].playMode == playModeBeatSlice;
    if (state == buttonPress)
    {
        padsBacklight.setFrontLayer(1, mtConfig.values.padsLightFront, pad);
        uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad, isBeatSlice);

        sequencer.handleNoteOn(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, -1, pad);
    }
    else if (state == buttonRelease)
    {
        padsBacklight.setFrontLayer(0, 0, pad);
        uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad, isBeatSlice);

        sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, 0, pad);
    }
}

void cSongEditor::handleEntryIcon()
{
    if (sequencer.isStop())
    {
        hideIcon();
    }
    else
    {
        if (sequencer.isSongMode())
        {
            startSongPlayer();
            refreshSongPlayerSelectedSlot();
        }
        else
        {
            lastSongListPos = sequencer.findSlotWithPattern();

            if (lastSongListPos >= 0)
            {
                selectedSongPosition = lastSongListPos;
                showIcon(iconLoop, lastSongListPos);
                startSongPlayer();
                refreshSongPlayerSelectedSlot();
            }
            else
            {
                hideIcon();
            }
        }
    }
}

void cSongEditor::switchToNewPattern()
{
    newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);

    mtProject.values.actualPattern = clamp(
      mtProject.song.playlist[SE->selectedSongPosition], PATTERN_INDEX_MIN, PATTERN_INDEX_MAX);

    newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
    sequencer.switchRamPatternsNow();
}

void cSongEditor::selectSpecificTrack(uint8_t track, uint8_t songPosition)
{
    songPlayerData.selection.isActive = 1;
    songPlayerData.selection.patternSelectionLength = SongRepeats::end(mtProject.song.playlist, songLength, songPosition) - SongRepeats::begin(mtProject.song.playlist, songLength, songPosition);
    songPlayerData.selection.trackSelectionLength = 1;
    songPlayerData.selection.startSongPosition = SongRepeats::begin(mtProject.song.playlist, songLength, songPosition);
    songPlayerData.selection.startTrack = track;

    refreshSongPlayerControl();
}

void cSongEditor::selectSongPosition(uint8_t songPosition)
{
    const int allTracksCount =
      SE->songPlayerData.normalTracksCount + SE->songPlayerData.midiTracksCount;

    songPlayerData.selection.isActive = 1;
    songPlayerData.selection.patternSelectionLength = SongRepeats::end(mtProject.song.playlist, songLength, songPosition) - SongRepeats::begin(mtProject.song.playlist, songLength, songPosition);
    songPlayerData.selection.trackSelectionLength = allTracksCount;
    songPlayerData.selection.startSongPosition = SongRepeats::begin(mtProject.song.playlist, songLength, songPosition);
    songPlayerData.selection.startTrack = 0;
}

void cSongEditor::walkOnSongPlayer(player_direction_t dir)
{
    auto& sel = songPlayerData.selection;
    const bool extend = buttons->isButtonPressed(interfaceButtonShift) && dir != playerHome;
    const auto* p = mtProject.song.playlist;
    int firstRow = SongRepeats::row(p, songLength, sel.startSongPosition);
    int lastRow = SongRepeats::row(p, songLength, sel.startSongPosition + std::max<int>(1, sel.patternSelectionLength) - 1);
    int row = SongRepeats::row(p, songLength, selectedSongPosition);
    int track = sel.startTrack;
    if (!extend) { firstRow = lastRow = row; sel.trackSelectionLength = 1; currSelDirection = selDirRight; }
    if (dir == playerHome) row = 0;
    if (dir == playerUp) --row;
    if (dir == playerDown) ++row;
    row = clamp<int>(row, 0, SongRepeats::count(p, songLength) - 1);
    if (dir == playerUp || dir == playerDown || dir == playerHome) {
        if (extend) {
            const int oldRow = SongRepeats::row(p, songLength, selectedSongPosition);
            const int anchor = oldRow == firstRow ? lastRow : firstRow;
            firstRow = std::min(anchor, row); lastRow = std::max(anchor, row);
        } else firstRow = lastRow = row;
    }
    const int allTracks = songPlayerData.normalTracksCount + songPlayerData.midiTracksCount;
    if (dir == playerLeft || dir == playerRight) {
        int lastTrack = track + sel.trackSelectionLength - 1;
        if (!extend) track = clamp<int>(track + (dir == playerLeft ? -1 : 1), 0, allTracks - 1);
        else {
            if (dir == playerLeft) {
                if (currSelDirection & selDirRight) {
                    if (lastTrack > track) --lastTrack;
                    else currSelDirection = selDirLeft;
                } else { currSelDirection = selDirLeft; if (track > 0) --track; }
            } else {
                if (currSelDirection & selDirLeft) {
                    if (lastTrack > track) ++track;
                    else currSelDirection = selDirRight;
                } else { currSelDirection = selDirRight; if (lastTrack < allTracks - 1) ++lastTrack; }
            }
            sel.trackSelectionLength = lastTrack - track + 1;
        }
    }
    sel.startTrack = track;
    sel.startSongPosition = SongRepeats::slot(p, songLength, firstRow);
    sel.patternSelectionLength = SongRepeats::end(p, songLength, SongRepeats::slot(p, songLength, lastRow)) - sel.startSongPosition;
    sel.isActive = 1;
    selectedSongPosition = SongRepeats::slot(p, songLength, row);
    refreshPatternsList();
    refreshSongPlayerSelectedSlot();
}

static void functSwitchModeSong(uint8_t state)
{
    if (state == buttonPress)
    {
        if (SE->selectedPlace == 2)
        {
            return;
        }
        SE->pressedSongButton = true;
        SE->lastSelectedPlace = SE->selectedPlace;
        SE->selectedPlace = 2;
        SE->activateLabelsBorder();
    }
    else if (state == buttonHold)
    {
        if (!SE->pressedSongButton)
        {
            SE->selectedPlace = 2;
            SE->exitOnButtonRelease = 1;
            SE->activateLabelsBorder();
        }
    }
    else if (state == buttonRelease)
    {
        if (SE->pressedSongButton)
        {
            SE->selectedPlace = SE->lastSelectedPlace;
        }
        SE->pressedSongButton = false;

        if (SE->exitOnButtonRelease)
        {
            SE->eventFunct(eventSwitchToPreviousModule, SE, 0, 0);
        }
        else
        {
            SE->activateLabelsBorder();
        }
    }
}

static void refreshCopyPasting()
{
    if (SE->isCopyingInProgress)
    {
        mtPopups.showProgressPopup("Copying");
        mtPopups.changePopupProgress(SE->currentCopyElement * 100 / SE->copyElementMax);
        display.processDisplayUpdateImmediate();

        uint8_t source =
          mtProject.song.playlist[SongRepeats::slot(mtProject.song.playlist, SE->songLength,
              SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->copyCurrentData.startSongPosition) + SE->currentCopyElement)];
        uint8_t destination =
          mtProject.song
            .playlist[SongRepeats::slot(mtProject.song.playlist, SE->songLength,
              SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) + SE->currentCopyElement)];

        if (source != 0 && destination != 0)
        {
            newFileManager.copySongTracks((char*)"Workspace",
                                          source,
                                          destination,
                                          SE->copyCurrentData.startTrack,
                                          SE->songPlayerData.selection.startTrack,
                                          SE->copyCurrentData.trackSelectionLength);
        }

        SE->currentCopyElement++;

        if (SE->currentCopyElement >= SE->copyElementMax)
        {
            mtPopups.hideProgressPopup();
            SE->refreshSongPlayerControl();
            SE->isBusy = 0;
            SE->isCopyingInProgress = 0;
            if (SE->clearSelectionAfterCopying)
            {
                SE->songPlayerData.selection.isActive = 0;
                SE->clearSelectionAfterCopying = false;
            }

            patternEditor.changeActualPattern(
              0); // to przeladuje wyswietlany pattern w patternEdicie jesli cos zmienialismy w
                  // aktualnym
        }
    }
}

static void functCopy()
{
    if (SE->isBusy)
    {
        return;
    }

    if (SE->selectedPlace == 0)
    {
        SE->isSomethingCopied = 1;
        memcpy(&SE->copyCurrentData, &SE->songPlayerData.selection, sizeof(song_selection_t));
    }
    else if (SE->selectedPlace == 1)
    {
        SE->isSomethingCopied = 1;
        SE->selectSongPosition(SE->selectedSongPosition);
        memcpy(&SE->copyCurrentData, &SE->songPlayerData.selection, sizeof(song_selection_t));

        SE->songPlayerData.selection.isActive = 0;
    }
}

static void functPaste()
{
    if (!SE->isSomethingCopied) return;
    SE->playlistUndoValid = false;
    if (SE->isBusy)
    {
        return;
    }

    if (SE->selectedPlace == 0)
    {
        if (SE->isSomethingCopied)
        {
            SE->currentCopyElement = 0;

            SE->isBusy = 1;
            SE->isCopyingInProgress = 1;
            SE->copyElementMax = SongRepeats::row(mtProject.song.playlist, SE->songLength,
              SE->copyCurrentData.startSongPosition + SE->copyCurrentData.patternSelectionLength - 1) -
              SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->copyCurrentData.startSongPosition) + 1;

            while ((SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) + SE->copyElementMax) >
                   SongRepeats::count(mtProject.song.playlist, SE->songLength))
            {
                SE->copyElementMax--;
            }
        }
    }
    else if (SE->selectedPlace == 1)
    {
        SE->selectSongPosition(SE->selectedSongPosition);

        SE->currentCopyElement = 0;

        SE->isBusy = 1;
        SE->isCopyingInProgress = 1;
        SE->copyElementMax = SongRepeats::row(mtProject.song.playlist, SE->songLength,
              SE->copyCurrentData.startSongPosition + SE->copyCurrentData.patternSelectionLength - 1) -
              SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->copyCurrentData.startSongPosition) + 1;

        while ((SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) + SE->copyElementMax) >
               SongRepeats::count(mtProject.song.playlist, SE->songLength))
        {
            SE->copyElementMax--;
        }

        SE->clearSelectionAfterCopying = true;
    }
}

static void functUndo()
{
    if (SE->isBusy)
    {
        return;
    }
    if (!SE->editMode)
    {
        return;
    }

    if (SE->undoPlaylistEdit()) return;
    newFileManager.undoSongPattern();
    SE->refreshSongPlayerControl();
    patternEditor.changeActualPattern(0); // przeladuj wyswietlany pattern w patternEdicie
}

static void functCopyPaste()
{
    if (!SE->editMode)
    {
        return;
    }

    if (!(SE->buttons->isButtonPressed(interfaceButtonShift)))
    {
        /*COPY*/
        functCopy();
    }
    else
    {
        /*PASTE*/
        functPaste();
    }
}

static void functInsert()
{
    if (SE->buttons->isButtonPressed(interfaceButtonShift))
    {
        switch (SE->selectedPlace)
        {
            case 0: SE->walkOnSongPlayer(playerHome); break;
            case 1: SE->changeSongPositionSelection(-300); break;
        }
    }
}

static void functDelete()
{
    if (SE->isBusy)
    {
        return;
    }

    SE->playlistUndoValid = false;
    if (SE->selectedPlace == 0)
    {
        SE->currentDeleteElement = 0;

        SE->isBusy = 1;
        SE->isDeletingInProgress = 1;
        SE->deleteElementMax = SongRepeats::row(mtProject.song.playlist, SE->songLength,
          SE->songPlayerData.selection.startSongPosition + SE->songPlayerData.selection.patternSelectionLength - 1) -
          SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) + 1;
    }
    else if (SE->selectedPlace == 1)
    {
        functDeleteSlot();
    }
    else if (SE->selectedPlace == 2)
    {
        SE->setDefaultGlobalTempo();
    }
}

static void refreshDeleting()
{
    if (SE->isDeletingInProgress)
    {
        mtPopups.showProgressPopup("Deleting");
        mtPopups.changePopupProgress(SE->currentDeleteElement * 100 / SE->deleteElementMax);
        display.processDisplayUpdateImmediate();

        uint8_t source =
          mtProject.song
            .playlist[SongRepeats::slot(mtProject.song.playlist, SE->songLength,
              SongRepeats::row(mtProject.song.playlist, SE->songLength, SE->songPlayerData.selection.startSongPosition) + SE->currentDeleteElement)];

        newFileManager.deleteTracks((char*)"Workspace",
                                    source,
                                    SE->songPlayerData.selection.startTrack,
                                    SE->songPlayerData.selection.trackSelectionLength);

        SE->currentDeleteElement++;

        if (SE->currentDeleteElement >= SE->deleteElementMax)
        {
            mtPopups.hideProgressPopup();
            SE->refreshSongPlayerControl();
            SE->isBusy = 0;
            SE->isDeletingInProgress = 0;

            patternEditor.changeActualPattern(
              0); // to przeladuje wyswietlany pattern w patternEdicie jesli cos usuwalismy z
                  // aktualnie tam zaladowanego
        }
    }
}

static void functMenu(uint8_t state)
{
    if (state == buttonPress)
    {
        mtMenuPopup.showModulePopup(SE);
    }
    else if (state == buttonRelease)
    {
        mtMenuPopup.hideModulePopup(SE);
    }
}
