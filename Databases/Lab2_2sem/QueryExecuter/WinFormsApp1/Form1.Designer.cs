
namespace WinFormsApp1
{
	partial class Form1
	{
		/// <summary>
		///  Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		///  Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		///  Required method for Designer support - do not modify
		///  the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			executeButton = new System.Windows.Forms.Button();
			showTablesButton = new System.Windows.Forms.Button();
			SuspendLayout();
			// 
			// executeButton
			// 
			executeButton.Location = new System.Drawing.Point(104, 148);
			executeButton.Name = "executeButton";
			executeButton.Size = new System.Drawing.Size(152, 23);
			executeButton.TabIndex = 3;
			executeButton.Text = "Редактирование таблиц";
			executeButton.UseVisualStyleBackColor = true;
			executeButton.Click += ExecuteButton_Click;
			// 
			// showTablesButton
			// 
			showTablesButton.Location = new System.Drawing.Point(104, 70);
			showTablesButton.Name = "showTablesButton";
			showTablesButton.Size = new System.Drawing.Size(152, 23);
			showTablesButton.TabIndex = 4;
			showTablesButton.Text = "Просмотр таблиц";
			showTablesButton.UseVisualStyleBackColor = true;
			showTablesButton.Click += ShowTablesButton_Click;
			// 
			// Form1
			// 
			AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			ClientSize = new System.Drawing.Size(390, 268);
			Controls.Add(showTablesButton);
			Controls.Add(executeButton);
			Name = "Form1";
			Text = "Form1";
			ResumeLayout(false);
		}

		#endregion
		private System.Windows.Forms.Button executeButton;
		private System.Windows.Forms.Button showTablesButton;
	}
}

