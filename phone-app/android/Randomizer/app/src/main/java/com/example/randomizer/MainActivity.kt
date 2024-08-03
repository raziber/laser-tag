package com.example.randomizer

import android.os.Bundle
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import org.w3c.dom.Text
import kotlin.random.Random

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)

        val rollButton = findViewById<Button>(R.id.rollButton)
        val resultsTextView = findViewById<TextView>(R.id.resultsTextView)
        val seekBar = findViewById<SeekBar>(R.id.seekBar)
        val selectedRangeTextView = findViewById<TextView>(R.id.selectedRangeTextView)
        val maxRangeTextView = findViewById<TextView>(R.id.seekBarMaxTextView)
        val minRangeTextView = findViewById<TextView>(R.id.seekBarMinTextView)

        maxRangeTextView.text = seekBar.max.toString()
        minRangeTextView.text = seekBar.min.toString()
        seekBar.progress = (seekBar.max + seekBar.min) / 2

        rollButton.setOnClickListener{
            val rand = Random.nextInt(seekBar.progress + 1)
            resultsTextView.text = rand.toString()
        }

        seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                selectedRangeTextView.text = progress.toString()
                val rangeXPos = seekBar?.thumb?.bounds?.exactCenterX() ?: 0f
                selectedRangeTextView.x = rangeXPos + 70
                selectedRangeTextView.visibility = TextView.VISIBLE
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {}

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                selectedRangeTextView.visibility = TextView.GONE
            }
        })

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}
