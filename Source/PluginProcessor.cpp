/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

auto getPhaserRateName() { return juce::String("Phaser RateHz");}
auto getPhaserCenterFreqName() { return juce::String("Phaser Center FreqHz");}
auto getPhaserDepthName() { return juce::String("Phaser Depth %");}
auto getPhaserFeedbackName() { return juce::String("Phaser Feedback %");}
auto getPhaserMixName() { return juce::String("Phaser Mix %");}
auto getPhaserBypassName() { return juce::String("Phaser Bypass");}

auto getChorusRateName() { return juce::String("Chorus RateHz");}
auto getChorusCenterDelayName() { return juce::String("Chorus Center DelayMs");}
auto getChorusDepthName() { return juce::String("Chorus Depth %");}
auto getChorusFeedbackName() { return juce::String("Chorus Feedback %");}
auto getChorusMixName() { return juce::String("Chorus Mix %");}
auto getChorusBypassName() { return juce::String("Chorus Bypass");}

auto getOverdriveSaturationName() { return juce::String("OverDrive Saturation");}
auto getOverdriveBypassName() { return juce::String("OverDrive Bypass");}

auto getLadderFilterModeName() { return juce::String("Ladder Filter Mode");}
auto getLadderFilterCutoffName() { return juce::String("Ladder Filter CutoffHz");}
auto getLadderFilterResonanceName() { return juce::String("Ladder Filter Resonance %");}
auto getLadderFilterDriveName() { return juce::String("Ladder Filter Drive");}
auto getLadderFilterBypassName() { return juce::String("Ladder Filter Bypass");}

auto getLadderFilterChoices()
{
    return juce::StringArray
    {
        "LPF12",
        "HPF12",
        "BPF12",
        "LPF24",
        "HPF24",
        "BPF24",
    };
}

auto getGeneralFilterChoices()
{
    return juce::StringArray
    {
        "Peak",
        "Bandpass",
        "Notch",
        "Allpass",
    };
}

auto getGeneralFilterModeName() { return juce::String("General Filter Mode");}
auto getGeneralFilterFreqName() { return juce::String("General Filter FreqHz");}
auto getGeneralFilterQualityName() { return juce::String("General Filter Quality");}
auto getGeneralFilterGainName() { return juce::String("General Filter Gain");}
auto getGeneralFilterBypassName() { return juce::String("General Filter Bypass");}

//==============================================================================
Project13AudioProcessor::Project13AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    dspOrder =
    {{
        DSP_Option::Phase,
        DSP_Option::Chorus,
        DSP_Option::OverDrive,
        DSP_Option::LadderFilter,
    }};
    
    /*
     cached params
     */
    
    auto floatParams = std::array
    {
        &phaserRateHz,
        &phaserCenterFreqHz,
        &phaserDepthPercent,
        &phaserFeedbackPercent,
        &phaserMixPercent,
        
        &chorusRateHz,
        &chorusCenterDelayMs,
        &chorusDepthPercent,
        &chorusFeedbackPercent,
        &chorusMixPercent,
        
        &overdriveSaturation,
        
        &ladderFilterCutoffHz,
        &ladderFilterResonance,
        &ladderFilterDrive,
        
        &generalFilterFreqHz,
        &generalFilterQuality,
        &generalFilterGain,
    };
    
    auto floatNameFuncs = std::array
    {
        &getPhaserRateName,
        &getPhaserCenterFreqName,
        &getPhaserDepthName,
        &getPhaserFeedbackName,
        &getPhaserMixName,
        
        &getChorusRateName,
        &getChorusCenterDelayName,
        &getChorusDepthName,
        &getChorusFeedbackName,
        &getChorusMixName,
        
        &getOverdriveSaturationName,
        
        &getLadderFilterResonanceName,
        &getLadderFilterResonanceName,
        &getLadderFilterDriveName,
        
        &getGeneralFilterFreqName,
        &getGeneralFilterQualityName,
        &getGeneralFilterGainName,
    };
    
    jassert ( floatParams.size() == floatNameFuncs.size());
    initCachedParams<juce::AudioParameterFloat*>(floatParams, floatNameFuncs);
    
    auto choiceParams = std::array
    {
        &ladderFilterMode,
        
        &generalFilterMode,
    };
    
    auto choiceNameFuncs = std::array
    {
        &getLadderFilterModeName,
        
        &getGeneralFilterModeName,
    };
    
    jassert ( choiceParams.size() == choiceNameFuncs.size());
    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceNameFuncs);
    
    auto boolParams = std::array
    {
        &phaserBypassBool,
        
        &chorusBypassBool,
        
        &overdriveBypassBool,
        
        &ladderFilterBypassBool,
        
        &generalFilterBypassBool,
    };
    
    auto boolNameFuncs = std::array
    {
        &getPhaserBypassName,
        
        &getChorusBypassName,
        
        &getOverdriveBypassName,
        
        &getLadderFilterBypassName,
        
        &getGeneralFilterBypassName
    };
    
    jassert ( boolParams.size() == boolNameFuncs.size());
    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);
}

Project13AudioProcessor::~Project13AudioProcessor()
{
}

//==============================================================================
const juce::String Project13AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Project13AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Project13AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Project13AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Project13AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Project13AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Project13AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Project13AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Project13AudioProcessor::getProgramName (int index)
{
    return {};
}

void Project13AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Project13AudioProcessor::MonoChannelDSP::prepare(const juce::dsp::ProcessSpec spec)
{
    jassert(spec.numChannels == 1);
    
    std::vector<juce::dsp::ProcessorBase*> dsp
    {
        &phaser,
        &chorus,
        &overdrive,
        &ladderFilter,
        &generalFilter,
    };
    
    for (auto p : dsp)
    {
        p->prepare(spec);
        p->reset();
    }
}

void Project13AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    
    leftChannel.prepare(spec);
    rightChannel.prepare(spec);
}

void Project13AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Project13AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


juce::AudioProcessorValueTreeState::ParameterLayout 
Project13AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    const int versionHint = 1;
    
    /*
     phaser:
     rate: (0-100) Hz
     depth: [0,1]
     center freq: Hz
     feedback: [-1, 1]
     mix: [0,1]
     */
    
    auto name = getPhaserRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f),
                0.2f, "Hz"));
    
    name = getPhaserDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
                0.5f, "%"));
    
    name = getPhaserCenterFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                1000.0f, "Hz"));
    
    name = getPhaserFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),
                0.0f, "%"));
    
    name = getPhaserMixName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
                0.05f, "%"));
    
    name = getPhaserBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>
               (juce::ParameterID {name, versionHint},
                name, false));
    
    /*
     chorus:
     rate: (0,100) Hz
     depth: [0,1]
     center delay: [1,100] ms
     feedback: [-1,1]
     mix: [0,1]
     */
    
    name = getChorusRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 100.f, 0.01f, 1.f),
                0.2f, "Hz"));
    
    name = getChorusDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
                0.05f, "%"));
    
    name = getChorusCenterDelayName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f),
                7.f, "ms"));
    
    name = getChorusFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),
                0.0f, "%"));
    
    name = getChorusMixName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
                0.05f, "%"));
    
    name = getChorusBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>
               (juce::ParameterID {name, versionHint},
                name, false));
    
    
    /*
     overdrive
     uses the drive portion of the LadderFilter clas for now
     drive: [1,100]
     */
    
    name = getOverdriveSaturationName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f),
                1.f, ""));
    
    name = getOverdriveBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>
               (juce::ParameterID {name, versionHint},
                name, false));
    
    /*
     ladder filter:
     mode: LadderFilterMode enum (int)
     cutoff: hz
     resonance: [0,1]
     drive: [1,100]
     */
    
    name = getLadderFilterModeName();
    auto choices = getLadderFilterChoices();
    layout.add(std::make_unique<juce::AudioParameterChoice>
               (juce::ParameterID {name, versionHint},
                name, choices, 0));
    
    name = getLadderFilterCutoffName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 1.f),
                20000.f, "Hz"));
    
    name = getLadderFilterResonanceName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(0.1f, 1.f, 0.01f, 1.f),
                0.1f, "%"));
    
    name = getLadderFilterDriveName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID {name, versionHint},
                name,
                juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f),
                1.f, ""));
    
    name = getLadderFilterBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>
               (juce::ParameterID {name, versionHint},
                name, false));
    
    /*
     general filter:
     https://docs.juce.com/develop/structdsp_1_1IIR_1_1Coefficients.html
     mode: peak, bandpass, notch, allpass
     freq: [20, 20000] hz in 1hz step
     Q: [0.1,10] in 0.05 steps
     gain: [-24,24] db in 0.5db steps
     */
    
    name = getGeneralFilterModeName();
    choices = getGeneralFilterChoices();
    layout.add(std::make_unique<juce::AudioParameterChoice>
               (juce::ParameterID {name, versionHint},
                name, choices, 0));
    
    name = getGeneralFilterFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID{name, versionHint},
                name,
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                750.f, "Hz"));
    
    name = getGeneralFilterQualityName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID{name, versionHint},
                name,
                juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                1.f, ""));
    
    name = getGeneralFilterGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>
               (juce::ParameterID{name, versionHint},
                name,
                juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                0.0f, ""));
    
    name = getGeneralFilterBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>
               (juce::ParameterID {name, versionHint},
                name, false));
    
    return layout;
}

void Project13AudioProcessor::MonoChannelDSP::updateDSPFromParams()
{
    phaser.dsp.setRate( p.phaserRateHz->get() );
    phaser.dsp.setCentreFrequency( p.phaserCenterFreqHz->get() );
    phaser.dsp.setDepth( p.phaserDepthPercent->get() );
    phaser.dsp.setFeedback( p.phaserFeedbackPercent->get() );
    phaser.dsp.setMix( p.phaserMixPercent->get() );
    
    chorus.dsp.setRate( p.chorusRateHz->get() );
    chorus.dsp.setDepth( p.chorusDepthPercent->get() );
    chorus.dsp.setCentreDelay( p.chorusCenterDelayMs->get() );
    chorus.dsp.setFeedback( p.chorusFeedbackPercent->get() );
    chorus.dsp.setMix( p.chorusMixPercent->get() );
    
    overdrive.dsp.setDrive( p.overdriveSaturation->get() );
    
    ladderFilter.dsp.setMode( static_cast<juce::dsp::LadderFilterMode>(p.ladderFilterMode->getIndex()));
    ladderFilter.dsp.setCutoffFrequencyHz( p.ladderFilterCutoffHz->get() );
    ladderFilter.dsp.setResonance( p.ladderFilterResonance->get() );
    ladderFilter.dsp.setDrive( p.ladderFilterDrive->get() );
    
    //TODO: update generalFilter coefficients here
}

void Project13AudioProcessor::MonoChannelDSP::process(juce::dsp::AudioBlock<float> block, const DSP_Order &dspOrder)
{
    // Now convert dspOrder into an array of pointers
    DSP_Pointers dspPointers;
    dspPointers.fill({});
    
    for(size_t i = 0; i < dspPointers.size(); i++)
    {
        switch (dspOrder[i])
        {
            case DSP_Option::Phase:
                dspPointers[i].processor = &phaser;
                dspPointers[i].bypassed = p.phaserBypassBool->get();
                break;
            case DSP_Option::Chorus:
                dspPointers[i].processor = &chorus;
                dspPointers[i].bypassed = p.chorusBypassBool->get();
                break;
            case DSP_Option::OverDrive:
                dspPointers[i].processor = &overdrive;
                dspPointers[i].bypassed = p.overdriveBypassBool->get();
                break;
            case DSP_Option::LadderFilter:
                dspPointers[i].processor = &ladderFilter;
                dspPointers[i].bypassed = p.ladderFilterBypassBool->get();
                break;
            case DSP_Option::GeneralFilter:
                dspPointers[i].processor = &generalFilter;
                dspPointers[i].bypassed = p.generalFilterBypassBool->get();
                break;
            case DSP_Option::END_OF_LIST:
                jassertfalse;
                break;
        }
    }
    
    //now process
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    for(size_t i = 0; i < dspPointers.size(); i++)
    {
        if(dspPointers[i].processor != nullptr)
        {
            juce::ScopedValueSetter<bool> svs(context.isBypassed, dspPointers[i].bypassed);
#if VERIFY_BYPASS_FUNCTIONALITY
            if (context.isBypassed)
            {
                jassertfalse;
            }
            
            if (dspPointers[i].processor == &generalFilter)
            {
                continue;
            }
#endif
            
            dspPointers[i].processor->process(context);
        }
    }
}

void Project13AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //[DONE]: add APVTS
    //[DONE]: create audio parameters for all dsp choices
    //[DONE]: bypass params for each DSP element
        //[DONE]: bypass implementation
    //[DONE]: update DSP here from audio parameters
       //TODO: update generalFilter coefficients
       //TODO: add smoothers for all param updates
    //[DONE]: save/load settings
    //[DONE]: save/load DSP order
        //[DONE]: filters are mono, not stereo.
    //TODO: Drag-To-Reorder GUI
    //TODO: GUI design for each DSP instance?
    //TODO: metering
    //[DONE]: prepare all DSP
    //TODO: wet/dry knob [BONUS] (can be controlled with setMix parameter of dsp effect classes)
    //TODO: mono & stereo versions [mono is BONUS]
    //TODO: modulators [BONUS]
    //TODO: thread-safe filter updating [BONUS]
    //TODO: pre/post filtering [BONUS]
    //TODO: delay module [BONUS]
    //TODO: negative feedback parameter [NICO]
    
    leftChannel.updateDSPFromParams();
    rightChannel.updateDSPFromParams();
    
    // Temp instance to pull into
    auto newDSPOrder = DSP_Order();
    
    // Try to pull
    while (dspOrderFifo.pull(newDSPOrder))
    {
#if VERIFY_BYPASS_FUNCTIONALITY
        jassertfalse;
#endif
    }
    
    // If you pulled, replace dspOrder
    if(newDSPOrder != DSP_Order())
        dspOrder = newDSPOrder;
    
    // now process
    auto block = juce::dsp::AudioBlock<float>(buffer);
    leftChannel.process(block.getSingleChannelBlock(0), dspOrder);
    rightChannel.process(block.getSingleChannelBlock(1), dspOrder);

}

//==============================================================================
bool Project13AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Project13AudioProcessor::createEditor()
{
//    return new Project13AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

template<>
struct juce::VariantConverter<Project13AudioProcessor::DSP_Order>
{
    static Project13AudioProcessor::DSP_Order fromVar (const juce::var &v)
    {
        using T = Project13AudioProcessor::DSP_Order;
        T dspOrder;
        
        jassert(v.isBinaryData());
        if (! v.isBinaryData())
        {
            dspOrder.fill(Project13AudioProcessor::DSP_Option::END_OF_LIST);
        }
        else
        {
            auto mb = *v.getBinaryData();
            
            juce::MemoryInputStream mis(mb, false);
            std::vector<int> arr;
            while(!mis.isExhausted())
            {
                arr.push_back(mis.readInt());
            }
            
            jassert(arr.size() == dspOrder.size());
            
            for(size_t i=0; i < dspOrder.size(); i++)
            {
                dspOrder[i] = static_cast<Project13AudioProcessor::DSP_Option>(arr[i]);
            }
        }
        
        return dspOrder;
    }
    
    static juce::var toVar (const Project13AudioProcessor::DSP_Order& t)
    {
        juce::MemoryBlock mb;
        //juce MOS uses scoping to complete writing to the memory block correctly
        {
            juce::MemoryOutputStream mos(mb, false);
            
            for(auto& v : t)
            {
                mos.writeInt(static_cast<int>(v));
            }
        }
        return mb;
    }
};

//==============================================================================
void Project13AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    apvts.state.setProperty("dspOrder", juce::VariantConverter<Project13AudioProcessor::DSP_Order>::toVar(dspOrder), nullptr);
    
    juce::MemoryOutputStream mos(destData, false);
    apvts.state.writeToStream(mos);
}

void Project13AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        if (apvts.state.hasProperty("dspOrder"))
        {
            auto order = juce::VariantConverter<Project13AudioProcessor::DSP_Order>::fromVar(apvts.state.getProperty("dspOrder"));
            dspOrderFifo.push(order);
        }
        DBG(apvts.state.toXmlString());
        
#if VERIFY_BYPASS_FUNCTIONALITY
        juce::Timer::callAfterDelay(1000, [this]()
                                    {
            DSP_Order order;
            order.fill(DSP_Option::LadderFilter);
            order[0] = DSP_Option::Chorus;
            chorusBypassBool->setValueNotifyingHost(1.f);
            dspOrderFifo.push(order);
        });
#endif
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Project13AudioProcessor();
}
