
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
			this.queryListLabel = new System.Windows.Forms.Label();
			this.queryListBox = new System.Windows.Forms.ListBox();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.executeButton = new System.Windows.Forms.Button();
			this.showTablesButton = new System.Windows.Forms.Button();
			this.queryLabel = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// queryListLabel
			// 
			this.queryListLabel.AutoSize = true;
			this.queryListLabel.Location = new System.Drawing.Point(382, 9);
			this.queryListLabel.Name = "queryListLabel";
			this.queryListLabel.Size = new System.Drawing.Size(102, 15);
			this.queryListLabel.TabIndex = 0;
			this.queryListLabel.Text = "Список запросов";
			// 
			// queryListBox
			// 
			this.queryListBox.FormattingEnabled = true;
			this.queryListBox.ItemHeight = 15;
			this.queryListBox.Items.AddRange(new object[] {
            "Номера авто клиентов",
            "Номер места каждого авто",
            "Лифты, вмещающие в себя больше 7.2 тонн",
			"Лифты, вмещающие в себя больше 5 тонн",
            "Полная характеристика этажей",
            "Номера мест на каждом этаже",
            "Клиенты со временем парковки больше 22 и фамилией Киркоров и места, " +
            "на которые они могут поставить свои авто",
            "Табло, к которому могут обращаться клиенты " +
			"на 2 этаже с парковкой больше 10 часов",
            "Вся информация о парковочных местах",
            "Вся информация о клиентах",
			"Информации о клиентах со временем парковки выше среднего",
			"Суммарной масса авто на стоянке, минимальное время парковки, максимальный " +
			"номер табло и количество клиентов с фамилией Киркоров",
			"Объединение паспортов и номеров машин из таблиц с последующей выборкой из них.",
			"Объединение паспортов и номеров машин из таблиц с последующей выборкой из них " +
			"паспортов без определенного номера",
			"Исключение одинаковых паспортов и вывод совпадающих номеров машин из таблиц",
			"Вывод всех клиентов, если среди них есть хоть один Киркоров",
			"Все клиенты, у которых время парковки больше, чем у первого припарковавшегося Синицына",
			"Все клиенты, у которых время парковки больше среднего и масса между максимальной " +
			"и минимальной массой авто на парковке",
			"Все клиенты парковки, у которых имя не Василий",
			"Фамилии и номер паспорта клиентов, номер которых содержит букву В",
			"Номера паспортов клиентов парковки и информация об этаже, на которых припаркованы авто",
			"Средние масса авто и время парковки у людей с фамилией Киркоров",
			"Общая информация по парковке",
			"Фамилии, номера паспорта, номера авто, номера места, у которых номер паспорта не содержит цифры 4",
			"Вывод фамилии, номера паспорта, номера авто, номера места, у которых номер паспорта содержит число 13",
            });
			this.queryListBox.Location = new System.Drawing.Point(12, 40);
			this.queryListBox.Name = "queryListBox";
			this.queryListBox.Size = new System.Drawing.Size(877, 154);
			this.queryListBox.TabIndex = 1;
			this.queryListBox.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(516, 255);
			this.textBox1.Multiline = true;
			this.textBox1.Name = "textBox1";
			this.textBox1.ReadOnly = true;
			this.textBox1.Size = new System.Drawing.Size(373, 109);
			this.textBox1.TabIndex = 2;
			// 
			// executeButton
			// 
			this.executeButton.Location = new System.Drawing.Point(130, 276);
			this.executeButton.Name = "executeButton";
			this.executeButton.Size = new System.Drawing.Size(87, 23);
			this.executeButton.TabIndex = 3;
			this.executeButton.Text = "Выполнить";
			this.executeButton.UseVisualStyleBackColor = true;
			this.executeButton.Click += new System.EventHandler(this.ExecuteButton_Click);
			// 
			// showTablesButton
			// 
			this.showTablesButton.Location = new System.Drawing.Point(97, 341);
			this.showTablesButton.Name = "showTablesButton";
			this.showTablesButton.Size = new System.Drawing.Size(152, 23);
			this.showTablesButton.TabIndex = 4;
			this.showTablesButton.Text = "Просмотр таблиц";
			this.showTablesButton.UseVisualStyleBackColor = true;
			this.showTablesButton.Click += new System.EventHandler(this.ShowTablesButton_Click);
			// 
			// queryLabel
			// 
			this.queryLabel.AutoSize = true;
			this.queryLabel.Location = new System.Drawing.Point(670, 214);
			this.queryLabel.Name = "queryLabel";
			this.queryLabel.Size = new System.Drawing.Size(47, 15);
			this.queryLabel.TabIndex = 5;
			this.queryLabel.Text = "Запрос";
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(901, 421);
			this.Controls.Add(this.queryLabel);
			this.Controls.Add(this.showTablesButton);
			this.Controls.Add(this.executeButton);
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.queryListBox);
			this.Controls.Add(this.queryListLabel);
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label queryListLabel;
		private System.Windows.Forms.ListBox queryListBox;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Button executeButton;
		private System.Windows.Forms.Button showTablesButton;
		private System.Windows.Forms.Label queryLabel;
	}
}

