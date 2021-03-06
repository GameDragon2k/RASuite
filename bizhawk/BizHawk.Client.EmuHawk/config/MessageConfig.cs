﻿using System;
using System.Drawing;
using System.Windows.Forms;

using BizHawk.Client.Common;

namespace BizHawk.Client.EmuHawk
{
	public partial class MessageConfig : Form
	{
		private int _dispFpSx = Global.Config.DispFPSx;
		private int _dispFpSy = Global.Config.DispFPSy;
		private int _dispFrameCx = Global.Config.DispFrameCx;
		private int _dispFrameCy = Global.Config.DispFrameCy;
		private int _dispLagx = Global.Config.DispLagx;
		private int _dispLagy = Global.Config.DispLagy;
		private int _dispInpx = Global.Config.DispInpx;
		private int _dispInpy = Global.Config.DispInpy;
		private int _lastInputColor = Global.Config.LastInputColor;
		private int _dispRecx = Global.Config.DispRecx;
		private int _dispRecy = Global.Config.DispRecy;
		private int _dispMultix = Global.Config.DispMultix;
		private int _dispMultiy = Global.Config.DispMultiy;
		private int _dispMessagex = Global.Config.DispMessagex;
		private int _dispMessagey = Global.Config.DispMessagey;
		private int _dispAutoholdx = Global.Config.DispAutoholdx;
		private int _dispAutoholdy = Global.Config.DispAutoholdy;

		private int _messageColor = Global.Config.MessagesColor;
		private int _alertColor = Global.Config.AlertMessageColor;
		private int _movieInput = Global.Config.MovieInput;
		
		private int _dispFpSanchor = Global.Config.DispFPSanchor;
		private int _dispFrameanchor = Global.Config.DispFrameanchor;
		private int _dispLaganchor = Global.Config.DispLaganchor;
		private int _dispInputanchor = Global.Config.DispInpanchor;
		private int _dispRecanchor = Global.Config.DispRecanchor;
		private int _dispMultiAnchor = Global.Config.DispMultianchor;
		private int _dispMessageAnchor = Global.Config.DispMessageanchor;
		private int _dispAutoholdAnchor = Global.Config.DispAutoholdanchor;

		private readonly Brush _brush = Brushes.Black;
		private int _px;
		private int _py;
		private bool _mousedown;

		public MessageConfig()
		{
			InitializeComponent();
		}

		private void MessageConfig_Load(object sender, EventArgs e)
		{
			SetMaxXY();
			MessageColorDialog.Color = Color.FromArgb(_messageColor);
			AlertColorDialog.Color = Color.FromArgb(_alertColor);
			LInputColorDialog.Color = Color.FromArgb(_lastInputColor);
			MovieInputColorDialog.Color = Color.FromArgb(_movieInput);
			SetColorBox();
			SetPositionInfo();
			StackMessagesCheckbox.Checked = Global.Config.StackOSDMessages;
		}

		private void SetMaxXY()
		{
			XNumeric.Maximum = Global.Emulator.VideoProvider.BufferWidth - 12;
			YNumeric.Maximum = Global.Emulator.VideoProvider.BufferHeight - 12;
			PositionPanel.Size = new Size(Global.Emulator.VideoProvider.BufferWidth + 2, Global.Emulator.VideoProvider.BufferHeight + 2);

			int width;
			if (Global.Emulator.VideoProvider.BufferWidth > 128)
			{
				width = Global.Emulator.VideoProvider.BufferWidth + 44;
			}
			else
			{
				width = 128 + 44;
			}

			PositionGroupBox.Size = new Size(width, Global.Emulator.VideoProvider.BufferHeight + 52);
		}

		private void SetColorBox()
		{
			_messageColor = MessageColorDialog.Color.ToArgb();
			ColorPanel.BackColor = MessageColorDialog.Color;
			ColorText.Text = String.Format("{0:X8}", _messageColor);

			_alertColor = AlertColorDialog.Color.ToArgb();
			AlertColorPanel.BackColor = AlertColorDialog.Color;
			AlertColorText.Text = String.Format("{0:X8}", _alertColor);

			_lastInputColor = LInputColorDialog.Color.ToArgb();
			LInputColorPanel.BackColor = LInputColorDialog.Color;
			LInputText.Text = String.Format("{0:X8}", _lastInputColor);

			_movieInput = MovieInputColorDialog.Color.ToArgb();
			MovieInputColor.BackColor = MovieInputColorDialog.Color;
			MovieInputText.Text = String.Format("{0:X8}", _movieInput);
		}

		private void SetAnchorRadio(int anchor)
		{
			switch (anchor)
			{
				default:
				case 0:
					TL.Checked = true; break;
				case 1:
					TR.Checked = true; break;
				case 2:
					BL.Checked = true; break;
				case 3:
					BR.Checked = true; break;
			}
		}

		private void SetPositionInfo()
		{
			if (FPSRadio.Checked)
			{
				XNumeric.Value = _dispFpSx;
				YNumeric.Value = _dispFpSy;
				_px = _dispFpSx;
				_py = _dispFpSy;
				SetAnchorRadio(_dispFpSanchor);
			}
			else if (FrameCounterRadio.Checked)
			{
				XNumeric.Value = _dispFrameCx;
				YNumeric.Value = _dispFrameCy;
				_px = _dispFrameCx;
				_py = _dispFrameCy;
				SetAnchorRadio(_dispFrameanchor);
			}
			else if (LagCounterRadio.Checked)
			{
				XNumeric.Value = _dispLagx;
				YNumeric.Value = _dispLagy;
				_px = _dispLagx;
				_py = _dispLagy;
				SetAnchorRadio(_dispLaganchor);
			}
			else if (InputDisplayRadio.Checked)
			{
				XNumeric.Value = _dispInpx;
				XNumeric.Value = _dispInpy;
				_px = _dispInpx;
				_py = _dispInpy;
				SetAnchorRadio(_dispInputanchor);
			}
			else if (MessagesRadio.Checked)
			{
				XNumeric.Value = _dispMessagex;
				YNumeric.Value = _dispMessagey;
				_px = _dispMessagex;
				_py = _dispMessagey;
				SetAnchorRadio(_dispMessageAnchor);
			}
			else if (RerecordsRadio.Checked)
			{
				XNumeric.Value = _dispRecx;
				YNumeric.Value = _dispRecy;
				_px = _dispRecx;
				_py = _dispRecy;
				SetAnchorRadio(_dispRecanchor);
			}
			else if (MultitrackRadio.Checked)
			{
				XNumeric.Value = _dispMultix;
				YNumeric.Value = _dispMultiy;
				_px = _dispMultix;
				_py = _dispMultiy;
				SetAnchorRadio(_dispMultiAnchor);
			}
			else if (AutoholdRadio.Checked)
			{
				XNumeric.Value = _dispAutoholdx;
				YNumeric.Value = _dispAutoholdy;
				_px = _dispAutoholdx;
				_py = _dispAutoholdy;
				SetAnchorRadio(_dispAutoholdAnchor);
			}

			PositionPanel.Refresh();
			XNumeric.Refresh();
			YNumeric.Refresh();
			SetPositionLabels();
		}

		private void SaveSettings()
		{
			Global.Config.DispFPSx = _dispFpSx;
			Global.Config.DispFPSy = _dispFpSy;
			Global.Config.DispFrameCx = _dispFrameCx;
			Global.Config.DispFrameCy = _dispFrameCy;
			Global.Config.DispLagx = _dispLagx;
			Global.Config.DispLagy = _dispLagy;
			Global.Config.DispInpx = _dispInpx;
			Global.Config.DispInpy = _dispInpy;
			Global.Config.DispRecx = _dispRecx;
			Global.Config.DispRecy = _dispRecy;
			Global.Config.DispMultix = _dispMultix;
			Global.Config.DispMultiy = _dispMultiy;
			Global.Config.DispMessagex = _dispMessagex;
			Global.Config.DispMessagey = _dispMessagey;
			Global.Config.DispAutoholdx = _dispAutoholdx;
			Global.Config.DispAutoholdy = _dispAutoholdy;

			Global.Config.MessagesColor = _messageColor;
			Global.Config.AlertMessageColor = _alertColor;
			Global.Config.LastInputColor = _lastInputColor;
			Global.Config.MovieInput = _movieInput;
			Global.Config.DispFPSanchor = _dispFpSanchor;
			Global.Config.DispFrameanchor = _dispFrameanchor;
			Global.Config.DispLaganchor = _dispLaganchor;
			Global.Config.DispInpanchor = _dispInputanchor;
			Global.Config.DispRecanchor = _dispRecanchor;
			Global.Config.DispMultianchor = _dispMultiAnchor;
			Global.Config.DispMessageanchor = _dispMessageAnchor;
			Global.Config.DispAutoholdanchor = _dispAutoholdAnchor;

			Global.Config.StackOSDMessages = StackMessagesCheckbox.Checked;
		}

		private void OK_Click(object sender, EventArgs e)
		{
			SaveSettings();
			GlobalWin.OSD.AddMessage("Message settings saved");
			Close();
		}

		private void MessageTypeRadio_CheckedChanged(object sender, EventArgs e)
		{
			SetPositionInfo();
		}

		private void XNumericChange()
		{
			_px = (int)XNumeric.Value;
			SetPositionLabels();
			PositionPanel.Refresh();
		}

		private void YNumericChange()
		{
			_py = (int)YNumeric.Value;
			SetPositionLabels();
			PositionPanel.Refresh();
		}

		private void Cancel_Click(object sender, EventArgs e)
		{
			GlobalWin.OSD.AddMessage("Message config aborted");
			Close();
		}

		private void PositionPanel_MouseEnter(object sender, EventArgs e)
		{
			Cursor = Cursors.Hand;
		}

		private void PositionPanel_MouseLeave(object sender, EventArgs e)
		{
			Cursor = Cursors.Default;
		}

		private void PositionPanel_Paint(object sender, PaintEventArgs e)
		{
			int x = 0;
			int y = 0;

			if (TL.Checked)
			{
				x = _px;
				y = _py;
			}
			else if (TR.Checked)
			{
				x = (int)XNumeric.Maximum - _px;
				y = _py;
			}
			else if (BL.Checked)
			{
				x = _px;
				y = (int)YNumeric.Maximum - _py;
			}
			else if (BR.Checked)
			{
				x = (int)XNumeric.Maximum - _px;
				y = (int)YNumeric.Maximum - _py;
			}

			var p = new Pen(_brush);
			e.Graphics.DrawLine(p, new Point(x, y), new Point(x + 8, y + 8));
			e.Graphics.DrawLine(p, new Point(x + 8, y), new Point(x, y + 8));
			e.Graphics.DrawRectangle(p, new Rectangle(x, y, 8, 8));
		}

		private void PositionPanel_MouseDown(object sender, MouseEventArgs e)
		{
			Cursor = Cursors.Arrow;
			_mousedown = true;
			SetNewPosition(e.X, e.Y);
		}

		private void PositionPanel_MouseUp(object sender, MouseEventArgs e)
		{
			Cursor = Cursors.Hand;
			_mousedown = false;
		}

		private void SetNewPosition(int mx, int my)
		{
			if (mx < 0) mx = 0;
			if (my < 0) my = 0;
			if (mx > XNumeric.Maximum) mx = (int)XNumeric.Maximum;
			if (my > YNumeric.Maximum) my = (int)YNumeric.Maximum;


			if (TL.Checked)
			{
				//Do nothing
			}
			else if (TR.Checked)
			{
				mx = (int)XNumeric.Maximum - mx;
			}
			else if (BL.Checked)
			{
				my = (int)YNumeric.Maximum - my;
			}
			else if (BR.Checked)
			{
				mx = (int)XNumeric.Maximum - mx;
				my = (int)YNumeric.Maximum - my;
			}

			XNumeric.Value = mx;
			YNumeric.Value = my;
			_px = mx;
			_py = my;

			PositionPanel.Refresh();
			SetPositionLabels();
		}

		private void PositionPanel_MouseMove(object sender, MouseEventArgs e)
		{
			if (_mousedown)
			{
				SetNewPosition(e.X, e.Y);
			}
		}

		private void SetPositionLabels()
		{
			if (FPSRadio.Checked)
			{
				_dispFpSx = _px;
				_dispFpSy = _py;
			}
			else if (FrameCounterRadio.Checked)
			{
				_dispFrameCx = _px;
				_dispFrameCy = _py;
			}
			else if (LagCounterRadio.Checked)
			{
				_dispLagx = _px;
				_dispLagy = _py;
			}
			else if (InputDisplayRadio.Checked)
			{
				_dispInpx = _px;
				_dispInpy = _py;
			}
			else if (RerecordsRadio.Checked)
			{
				_dispRecx = _px;
				_dispRecy = _py;
			}
			else if (MultitrackRadio.Checked)
			{
				_dispMultix = _px;
				_dispMultiy = _py;
			}
			else if (MessagesRadio.Checked)
			{
				_dispMessagex = _px;
				_dispMessagey = _py;
			}
			else if (AutoholdRadio.Checked)
			{
				_dispAutoholdx = _px;
				_dispAutoholdy = _py;
			}

			FpsPosLabel.Text = _dispFpSx + ", " + _dispFpSy;
			FCLabel.Text = _dispFrameCx + ", " + _dispFrameCy;
			LagLabel.Text = _dispLagx + ", " + _dispLagy;
			InpLabel.Text = _dispInpx + ", " + _dispInpy;
			RerecLabel.Text = _dispRecx + ", " + _dispRecy;
			MultitrackLabel.Text = _dispMultix + ", " + _dispMultiy;
			MessLabel.Text = _dispMessagex + ", " + _dispMessagey;
			AutoholdLabel.Text = _dispAutoholdx + ", " + _dispAutoholdy;
		}

		private void ResetDefaultsButton_Click(object sender, EventArgs e)
		{
			Global.Config.DispFPSx = 0;
			Global.Config.DispFPSy = 0;
			Global.Config.DispFrameCx = 0;
			Global.Config.DispFrameCy = 12;
			Global.Config.DispLagx = 0;
			Global.Config.DispLagy = 36;
			Global.Config.DispInpx = 0;
			Global.Config.DispInpy = 24;
			Global.Config.DispRecx = 0;
			Global.Config.DispRecy = 48;
			Global.Config.DispMultix = 0;
			Global.Config.DispMultiy = 14;
			Global.Config.DispMessagex = 3;
			Global.Config.DispMessagey = 0;
			Global.Config.DispAutoholdx = 0;
			Global.Config.DispAutoholdy = 0;

			Global.Config.MessagesColor = -1;
			Global.Config.AlertMessageColor = -65536;
			Global.Config.LastInputColor = -23296;
			Global.Config.MovieInput = -8355712;

			_messageColor = Global.Config.MessagesColor;
			_alertColor = Global.Config.AlertMessageColor;
			_lastInputColor = Global.Config.LastInputColor;
			_movieInput = Global.Config.MovieInput;

			MessageColorDialog.Color = Color.FromArgb(_messageColor);
			AlertColorDialog.Color = Color.FromArgb(_alertColor);
			LInputColorDialog.Color = Color.FromArgb(_lastInputColor);
			MovieInputColorDialog.Color = Color.FromArgb(_movieInput);

			Global.Config.DispFPSanchor = 0;
			Global.Config.DispFrameanchor = 0;
			Global.Config.DispLaganchor = 0;
			Global.Config.DispInpanchor = 0;
			Global.Config.DispRecanchor = 0;
			Global.Config.DispMultianchor = 1;
			Global.Config.DispMessageanchor = 2;
			Global.Config.DispAutoholdanchor = 1;

			_dispFpSx = Global.Config.DispFPSx;
			_dispFpSy = Global.Config.DispFPSy;
			_dispFrameCx = Global.Config.DispFrameCx;
			_dispFrameCy = Global.Config.DispFrameCy;
			_dispLagx = Global.Config.DispLagx;
			_dispLagy = Global.Config.DispLagy;
			_dispInpx = Global.Config.DispInpx;
			_dispInpy = Global.Config.DispInpy;
			_dispRecx = Global.Config.DispRecx;
			_dispRecy = Global.Config.DispRecy;
			_dispMultix = Global.Config.DispMultix;
			_dispMultiy = Global.Config.DispMultiy;
			_dispMessagex = Global.Config.DispMessagex;
			_dispMessagey = Global.Config.DispMessagey;
			_dispAutoholdx = Global.Config.DispAutoholdx;
			_dispAutoholdy = Global.Config.DispAutoholdy;

			_dispFpSanchor = Global.Config.DispFPSanchor;
			_dispFrameanchor = Global.Config.DispFrameanchor;
			_dispLaganchor = Global.Config.DispLaganchor;
			_dispInputanchor = Global.Config.DispInpanchor;
			_dispRecanchor = Global.Config.DispRecanchor;
			_dispMultiAnchor = Global.Config.DispMultianchor;
			_dispMessageAnchor = Global.Config.DispMessageanchor;
			_dispAutoholdAnchor = Global.Config.DispAutoholdanchor;

			SetMaxXY();
			SetColorBox();
			SetPositionInfo();

			StackMessagesCheckbox.Checked = Global.Config.StackOSDMessages = true;
		}

		private void SetAnchorValue(int value)
		{
			if (FPSRadio.Checked)
			{
				_dispFpSanchor = value;
			}
			else if (FrameCounterRadio.Checked)
			{
				_dispFrameanchor = value;
			}
			else if (LagCounterRadio.Checked)
			{
				_dispLaganchor = value;
			}
			else if (InputDisplayRadio.Checked)
			{
				_dispInputanchor = value;
			}
			else if (MessagesRadio.Checked)
			{
				_dispMessageAnchor = value;
			}
			else if (RerecordsRadio.Checked)
			{
				_dispRecanchor = value;
			}
			else if (MultitrackRadio.Checked)
			{
				_dispMultiAnchor = value;
			}
			else if (AutoholdRadio.Checked)
			{
				_dispAutoholdAnchor = value;
			}
		}

		private void TL_CheckedChanged(object sender, EventArgs e)
		{
			if (TL.Checked)
			{
				SetAnchorValue(0);
			}
			PositionPanel.Refresh();
		}

		private void TR_CheckedChanged(object sender, EventArgs e)
		{
			if (TR.Checked)
			{
				SetAnchorValue(1);
			}
			PositionPanel.Refresh();
		}

		private void BL_CheckedChanged(object sender, EventArgs e)
		{
			if (BL.Checked)
			{
				SetAnchorValue(2);
			}
			PositionPanel.Refresh();
		}

		private void BR_CheckedChanged(object sender, EventArgs e)
		{
			if (BR.Checked)
			{
				SetAnchorValue(3);
			}
			PositionPanel.Refresh();
		}

		private void XNumeric_Click(object sender, EventArgs e)
		{
			XNumericChange();
		}

		private void YNumeric_Click(object sender, EventArgs e)
		{
			YNumericChange();
		}

		private void ColorPanel_Click(object sender, EventArgs e)
		{
			if (MessageColorDialog.ShowDialog() == DialogResult.OK)
			{
				SetColorBox();
			}
		}

		private void AlertColorPanel_Click(object sender, EventArgs e)
		{
			if (AlertColorDialog.ShowDialog() == DialogResult.OK)
			{
				SetColorBox();
			}
		}

		private void LInputColorPanel_Click(object sender, EventArgs e)
		{
			if (LInputColorDialog.ShowDialog() == DialogResult.OK)
			{
				SetColorBox();
			}
		}

		private void MovieInputColor_Click(object sender, EventArgs e)
		{
			if (MovieInputColorDialog.ShowDialog() == DialogResult.OK)
			{
				SetColorBox();
			}
		}

		private void XNumeric_KeyUp(object sender, KeyEventArgs e)
		{
			XNumericChange();
		}

		private void YNumeric_KeyUp(object sender, KeyEventArgs e)
		{
			YNumericChange();
		}
	}
}
