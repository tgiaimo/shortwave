#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // gain slider
    gainSlider = std::make_unique<juce::Slider> (juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    addAndMakeVisible (gainSlider.get());

    // gain label
    gainLabel = std::make_unique<juce::Label> ("", "gain");
    addAndMakeVisible (gainLabel.get());
    gainLabel->attachToComponent (gainSlider.get(), false);
    gainLabel->setJustificationType (juce::Justification::centred);


    gainAttachment = std::make_unique<Attachment> (processorRef.apvts, "gain", *gainSlider);
    
    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto area = getLocalBounds();
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromTop (150), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromBottom(50);
    inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre(100, 50));
    auto bounds = getLocalBounds();

    // Give 40 pixels of space for the title
    auto textBounds = bounds.removeFromTop (40);

    // Give 40 pixels of margin on all sides so we don't paint to the edges of the background
    bounds.reduce (40, 40);

    juce::Grid grid;

    // TrackInfo is a "space"
    using Track = juce::Grid::TrackInfo;

    // Fr is how big the space is relative to other spaces
    using Fr = juce::Grid::Fr;

    // 4 columns x 2 rows
    grid.templateColumns = { Track (Fr (1)), Track (Fr (1)), Track (Fr (1)), Track (Fr (1)) };
    grid.templateRows = { Track (Fr (1)), Track (Fr (1)) };

    // Margin between each space
    grid.rowGap = juce::Grid::Px (20);
    grid.columnGap = juce::Grid::Px (20);

    // Add components to the grid
    grid.items.add (gainSlider.get());

    // Perform the layout on the bounds
    grid.performLayout (bounds);
}
