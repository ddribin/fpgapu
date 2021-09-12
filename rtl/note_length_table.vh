
// This is meant to be included inside a module, so don't use include guards

function [4:0] note_len;
  input [4:0] i_len;
  begin
    note_len = i_len - 1;
  end
endfunction

`define LEN_THIRTY_SECOND note_len(1)
`define NOTE_LEN_SIXTEENTH note_len(2)
`define NOTE_LEN_EIGHTH note_len(4)
`define NOTE_LEN_QUARTER note_len(8)
`define NOTE_LEN_HALF note_len(16)
`define NOTE_LEN_WHOLE note_len(32)
